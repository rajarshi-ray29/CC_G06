let a = 0;
let b = 1;
let c = 2;
let d = 3;
let e = 4;
let f = 5;
f = (a ? b : c) ? d : e;
dbg f;

// output: (begin (let a 0) (let b 1) (let c 2) (let d 3) (let e 4) (let f 5) (assign f (?: (?: a b c) d e)) (dbg f))
// 3