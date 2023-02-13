#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdio>
#include <cstring>
#include <fstream>

#include "parser.hh"
#include "ast.hh"
#include "llvmcodegen.hh"

extern FILE *yyin;
extern int yylex();
extern char *yytext;

extern FILE *fooin;
extern FILE *fooout;
extern int foolex();
extern char *footext;

extern std::string key;
extern std::unordered_map<std::string, std::string> map;

NodeStmts *final_values;

#define ARG_OPTION_L 0
#define ARG_OPTION_P 1
#define ARG_OPTION_S 2
#define ARG_OPTION_O 3
#define ARG_FAIL -1

int parse_arguments(int argc, char *argv[]) {
	if (argc == 3 || argc == 4) {
		if (strlen(argv[2]) == 2 && argv[2][0] == '-') {
			if (argc == 3) {
				switch (argv[2][1]) {
					case 'l':
					return ARG_OPTION_L;

					case 'p':
					return ARG_OPTION_P;

					case 's':
					return ARG_OPTION_S;
				}
			} else if (argv[2][1] == 'o') {
				return ARG_OPTION_O;
			}
		}
	} 
	
	std::cerr << "Usage:\nEach of the following options halts the compilation process at the corresponding stage and prints the intermediate output:\n\n";
	std::cerr << "\t`./bin/base <file_name> -l`, to tokenize the input and print the token stream to stdout\n";
	std::cerr << "\t`./bin/base <file_name> -p`, to parse the input and print the abstract syntax tree (AST) to stdout\n";
	std::cerr << "\t`./bin/base <file_name> -s`, to compile the file to LLVM assembly and print it to stdout\n";
	std::cerr << "\t`./bin/base <file_name> -o <output>`, to compile the file to LLVM bitcode and write to <output>\n";
	return ARG_FAIL;
}

bool cycle_check(std::unordered_map<std::string, std::string> m) {
	for(auto i: m) {
		std::string ptr = i.first;
		while(m.find(ptr) != m.end()) {
			ptr = m[ptr];
			if(ptr == i.first) return true;
		}
	}
	return false;
}

void preprocess() {
	// Actual Pre
	int count;
	int token;
	std::string contents;
	
	// Run preprocessor until no more macros can be expanded
	// Preprocessor works on a "temp" file which is removed at the end
	do {
		fooin = fopen("temp", "r");
		count = 0;
		token = 0;
		contents = "";

		// Run lexer on program (macro replacing and comment removal)
		do {
			token = foolex();
			std::string temp = footext;

			// Every time a macro is added, check for cycles
			if(token == 5 && cycle_check(map)) {
				std::cerr<<"Cycle detected in #def statements"<<std::endl;
				remove("temp");
				fclose(fooin);
				exit(1);
			}

			// Every time a word is taken in check if it matches macro
			if(token == 3 && map.find(temp) != map.end()) {
				count++;
				temp = map[temp];
			}
			contents += temp;

		} while(token != 0);

		std::ofstream otemp("temp");
		otemp<<contents;
		otemp.close();
	} while(count > 0);

	fooin = fopen("temp", "r");
	contents = "";
	do {
		token = foolex();
		std::string temp = footext;
		if(token != 1 && token != 2 && token != 5)
			contents += temp;

	} while(token != 0);

	// Printing final preprocessed code
	std::cout<<"PRE"<<std::endl<<contents<<std::endl;
	
	fclose(fooin);

	std::ofstream ofile("temp");
	ofile<<contents;
	ofile.close();
}

int main(int argc, char *argv[]) {
	int arg_option = parse_arguments(argc, argv);
	if (arg_option == ARG_FAIL) {
		exit(1);
	}

	// Copying main file to temp for preprocessing
	std::string file_name(argv[1]);

	std::ifstream itemp(file_name);
	std::ofstream otemp("temp");
	std::string line;
	while (getline(itemp, line)) {
		otemp << line << std::endl;
	}
	itemp.close();
	otemp.close();

	preprocess();

	// Main Lexer and Parser
	yyin = fopen("temp", "r");

	// For debugging, prints tokens
	if (arg_option == ARG_OPTION_L) {
		extern std::string token_to_string(int token, const char *lexeme);

		while (true) {
			int token = yylex();
			if (token == 0) {
				break;
			}

			std::cout << token_to_string(token, yytext) << "\n";
		}
		fclose(yyin);
		return 0;
	}

    final_values = nullptr;

	// Actual lex and parse
	yyparse();

	fclose(yyin);
	remove("temp");

	if(final_values) {
		if (arg_option == ARG_OPTION_P) {
			std::cout << final_values->to_string() << std::endl;
			return 0;
		}
		
        llvm::LLVMContext context;
		LLVMCompiler compiler(&context, "base");
		compiler.compile(final_values);
        if (arg_option == ARG_OPTION_S) {
			compiler.dump();
        } else {
            compiler.write(std::string(argv[3]));
		}
	} else {
	 	std::cerr << "empty program";
	}

    return 0;
}