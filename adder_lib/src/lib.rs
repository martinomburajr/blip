#[unsafe(no_mangle)]
#[inline(never)]
pub fn adder(num: i32) -> i32 {
    if num < 2 { return num; }
    let mut prev = 2;
    let mut sum = 2;
    for _ in 0..num {
        sum += prev;
        prev = sum;
    }
    prev
}
