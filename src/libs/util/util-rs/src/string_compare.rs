pub fn ignore_case_find(s: &str, pat: &str, start: usize) -> Option<usize> {
    let mut n = start;
    while n < s.len() && !s.is_char_boundary(n) {
        n += 1;
    }

    if n >= s.len() {
        return None;
    }

    let s_lowercase = (&s[n..]).to_lowercase();
    let pat_lowercase = pat.to_lowercase();
    s_lowercase.find(&pat_lowercase).map(|index| index + n)
}

#[cfg(test)]
mod tests {
    use crate::string_compare::ignore_case_find;
    use std::fs::File;
    use std::io::{self, BufRead};
    use std::path::Path;

    #[test]
    fn index_of_test() {
        let mut test_data = read_lines("control_data/ignore_case_index_of.txt").unwrap();
        while let Some(s1) = test_data.next() {
            let s1 = s1.unwrap();
            let s2 = test_data.next().unwrap().unwrap();
            let start = test_data
                .next()
                .unwrap()
                .unwrap()
                .parse::<usize>()
                .expect("Not usize");
            let expected_string = test_data.next().unwrap().unwrap();
            let expected = match expected_string.parse::<i32>() {
                Ok(v) => v,
                Err(_) => panic!("<{}> is not i32", expected_string),
            };

            let result = ignore_case_find(&s1, &s2, start)
                .map(|i| i as i32)
                .unwrap_or(-1);
            assert_eq!(
                result, expected,
                "s1: <{}>, s2: <{}>, start: <{}>",
                &s1, &s2, start
            );
        }
    }

    fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
    where
        P: AsRef<Path>,
    {
        let file = File::open(filename)?;
        Ok(io::BufReader::new(file).lines())
    }
}
