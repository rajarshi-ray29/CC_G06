let a = 2;
let b = 10;
let c = 11;
let d = 1;
d = a ? b : c;
dbg d; 

// output: (begin (let a 2) (let b 10) (let c 11) (let d 1) (assign d (?: a b c)) (dbg d))
// 10