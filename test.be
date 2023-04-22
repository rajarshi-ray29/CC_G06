let a : long = 12000;
b = a;
dbg b; 

// output: (begin (let a 3) (let b 1) (assign b a) (dbg b))
// 3