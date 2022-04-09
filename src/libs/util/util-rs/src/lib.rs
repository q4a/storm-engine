use std::fs::File;
use std::io::Write;

#[no_mangle]
pub extern "C" fn hello_world() {
    let mut file = File::create("rust.txt").unwrap();
    writeln!(&mut file, "Hello from Rust").unwrap();
}

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        let result = 2 + 2;
        assert_eq!(result, 4);
    }
}
