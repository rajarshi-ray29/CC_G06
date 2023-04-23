let a = 10;
dbg 2 ? (0 ? 20 : 30) : 8;

// output : (begin (let a 10) (dbg (?: 2 (?: 0 20 30) 8)))