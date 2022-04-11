pub fn ignore_case_equal(first: &str, second: &str) -> bool {
    let first_uppercase = first.to_uppercase();
    let second_uppercase = second.to_uppercase();
    first_uppercase == second_uppercase
}

pub fn ignore_case_less(first: &str, second: &str) -> bool {
    let first_uppercase = first.to_uppercase();
    let second_uppercase = second.to_uppercase();
    first_uppercase < second_uppercase
}

pub fn ignore_case_less_or_equal(first: &str, second: &str) -> bool {
    let first_uppercase = first.to_uppercase();
    let second_uppercase = second.to_uppercase();
    first_uppercase <= second_uppercase
}

pub fn ignore_case_equal_first_n(first: &str, second: &str, n: usize) -> bool {
    if first.len() < n || second.len() < n {
        if first.len() != second.len() {
            false
        } else {
            ignore_case_equal(first, second)
        }
    } else {
        let sliced_first = &first[..n];
        let sliced_second = &second[..n];
        ignore_case_equal(sliced_first, sliced_second)
    }
}

pub fn wildcmp(wild: &str, string: &str) -> bool {
    regex::Regex::new(wild)
        .map(|pattern| pattern.is_match(string))
        .unwrap_or(false)
}

pub fn wildicmp(wild: &str, string: &str) -> bool {
    let wild_lowercase = wild.to_lowercase();
    let string_lowercase = string.to_lowercase();
    wildcmp(&wild_lowercase, &string_lowercase)
}

#[cfg(test)]
mod tests {
    use crate::string_compare::{ignore_case_equal, ignore_case_equal_first_n, ignore_case_less};

    #[test]
    fn ignore_case_equal_test() {
        let str_lowercase = "mystring";
        let str_uppercase = "MYSTRING";
        let str_mixedcase = "mYsTrInG";

        let str_other = "m_string";
        let str_long = "mystrings";

        // Strings with same case should be considered equal
        assert!(ignore_case_equal(str_lowercase, str_lowercase));
        assert!(ignore_case_equal(str_uppercase, str_uppercase));
        assert!(ignore_case_equal(str_mixedcase, str_mixedcase));

        // Strings with different case should be considered equal
        assert!(ignore_case_equal(str_lowercase, str_uppercase));
        assert!(ignore_case_equal(str_lowercase, str_mixedcase));
        assert!(ignore_case_equal(str_uppercase, str_mixedcase));

        // Strings with different value should be considered not equal
        assert!(!ignore_case_equal(str_lowercase, str_other));

        // Strings with different length should be considered not equal
        assert!(!ignore_case_equal(str_lowercase, str_long));

        // Compare only count number of character at most
        assert!(ignore_case_equal_first_n(str_lowercase, str_long, 8));
        assert!(ignore_case_equal_first_n(str_long, str_lowercase, 8));
        assert!(!ignore_case_equal_first_n(".txt1", ".txt2", 5));
    }

    #[test]
    fn ignore_case_less_test() {
        let str_lowercase = "mystring";
        let str_uppercase = "MYSTRING";
        let str_mixedcase = "mYsTrInG";

        let str_long = "mystrings";

        // Identical string should not be less
        assert!(!ignore_case_less(str_lowercase, str_lowercase));
        assert!(!ignore_case_less(str_uppercase, str_uppercase));
        assert!(!ignore_case_less(str_mixedcase, str_mixedcase));

        // Lowercase should not be considered before uppercase
        assert!(!ignore_case_less(str_lowercase, str_uppercase));
        assert!(!ignore_case_less(str_uppercase, str_lowercase));
        assert!(!ignore_case_less(str_lowercase, str_mixedcase));
        assert!(!ignore_case_less(str_mixedcase, str_lowercase));
        assert!(!ignore_case_less(str_mixedcase, str_uppercase));
        assert!(!ignore_case_less(str_uppercase, str_mixedcase));

        // Characters that appear earlier in the alphabet should be cosidered less than character after it
        assert!(ignore_case_less("A", "b"));
        assert!(!ignore_case_less("c", "B"));

        // Shorter string should be considered before longer string, when they have the same starting sequence
        assert!(ignore_case_less(str_lowercase, str_long));
        assert!(!ignore_case_less(str_long, str_lowercase));
    }
}
