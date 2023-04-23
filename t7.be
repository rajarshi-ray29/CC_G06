let a = 0;
let b = 3;
b = a ? 7 : a + 1;

// output : (begin (let a 0) (let b 3) (assign b (?: a 7 (+ a 1))))