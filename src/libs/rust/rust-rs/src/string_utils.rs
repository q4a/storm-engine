use std::{cmp::Ordering, collections::hash_map::DefaultHasher, hash::Hasher};

use crate::common::DEFAULT_LOGGER;

/// Case-insensitive search of `pattern`'s position in `s` starting from `start` byte.
/// Since `start` can point inside UTF-8 code points, we need to find near char boundary first.
pub fn find_ignore_case(s: &str, pattern: &str, start: usize) -> Option<usize> {
    let mut n = start;
    while n < s.len() && !s.is_char_boundary(n) {
        n += 1;
    }

    if n >= s.len() {
        return None;
    }

    let s_lowercase = (&s[n..]).to_lowercase();
    let pattern_lowercase = pattern.to_lowercase();
    s_lowercase.find(&pattern_lowercase).map(|index| index + n)
}

/// Case-insensitive check that `s` starts with `pattern`
pub fn starts_with_ignore_case(s: &str, pattern: &str) -> bool {
    let s_lowercase = s.to_lowercase();
    let pattern_lowercase = pattern.to_lowercase();
    s_lowercase.starts_with(&pattern_lowercase)
}

/// Case-insensitive check that `s` ends with `pattern`
pub fn ends_with_ignore_case(s: &str, pattern: &str) -> bool {
    let s_lowercase = s.to_lowercase();
    let pattern_lowercase = pattern.to_lowercase();
    s_lowercase.ends_with(&pattern_lowercase)
}

/// Case-insensitive comparison of `s1` and `s2`
pub fn compare_ignore_case(s1: &str, s2: &str) -> Ordering {
    let s1_lowercase = s1.to_lowercase();
    let s2_lowercase = s2.to_lowercase();

    s1_lowercase.cmp(&s2_lowercase)
}

/// Case-insensitive check that `s1` is equal to `s2`
pub fn equal_ignore_case(s1: &str, s2: &str) -> bool {
    matches!(compare_ignore_case(s1, s2), Ordering::Equal)
}

/// Case-insensitive check that `s1` is less than `s2`
pub fn less_ignore_case(s1: &str, s2: &str) -> bool {
    matches!(compare_ignore_case(s1, s2), Ordering::Less)
}

/// Case-insensitive check that `s1` is less than or equal to `s2`
pub fn less_or_equal_ignore_case(s1: &str, s2: &str) -> bool {
    !matches!(compare_ignore_case(s1, s2), Ordering::Greater)
}

/// Case-insensitive check that `s1` is equal to `pattern` glob
pub fn glob_ignore_case(s: &str, pattern: &str) -> bool {
    let s_lowercase = s.to_lowercase();
    let pattern_lowercase = pattern.to_lowercase();
    let matcher = match globset::Glob::new(&pattern_lowercase) {
        Ok(glob) => glob.compile_matcher(),
        Err(e) => {
            log::error!(
                target: DEFAULT_LOGGER,
                "Couldn't compile <{}> glob, reason: {}",
                pattern,
                &e
            );
            panic!("Don't know what to do");
        }
    };

    matcher.is_match(&s_lowercase)
}

/// Case-dependent hashing
pub fn hash(s: &str) -> u64 {
    let mut hasher = DefaultHasher::new();
    hasher.write(s.as_bytes());
    hasher.finish()
}

/// Case-independent hashing
pub fn hash_ignore_case(s: &str) -> u64 {
    hash(&s.to_lowercase())
}

mod export {
    use std::{
        borrow::Borrow,
        os::raw::{c_char, c_int},
    };

    use crate::common::ffi::{c_char_to_str, size_t, uint64_t, win1251_char_to_str};

    use super::{
        ends_with_ignore_case, equal_ignore_case, find_ignore_case, glob_ignore_case, hash,
        hash_ignore_case, less_ignore_case, less_or_equal_ignore_case, starts_with_ignore_case,
    };

    #[no_mangle]
    pub unsafe extern "C" fn ffi_find_ignore_case(
        s: *const c_char,
        pattern: *const c_char,
        start: size_t,
    ) -> c_int {
        let s = c_char_to_str(s);
        let pattern = c_char_to_str(pattern);
        find_ignore_case(s, pattern, start)
            .map(|index| index as i32)
            .unwrap_or(-1)
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_starts_with_ignore_case(
        s: *const c_char,
        pattern: *const c_char,
    ) -> bool {
        let s = c_char_to_str(s);
        let pattern = c_char_to_str(pattern);

        starts_with_ignore_case(s, pattern)
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_ends_with_ignore_case(
        s: *const c_char,
        pattern: *const c_char,
    ) -> bool {
        let s = c_char_to_str(s);
        let pattern = c_char_to_str(pattern);

        ends_with_ignore_case(s, pattern)
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_equal_ignore_case(
        first: *const c_char,
        second: *const c_char,
    ) -> bool {
        let first = c_char_to_str(first);
        let second = c_char_to_str(second);

        equal_ignore_case(first, second)
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_win1251_equal_ignore_case(
        first: *const c_char,
        second: *const c_char,
    ) -> bool {
        let first = win1251_char_to_str(first);
        let second = win1251_char_to_str(second);

        equal_ignore_case(first.borrow(), second.borrow())
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_equal(first: *const c_char, second: *const c_char) -> bool {
        let first = c_char_to_str(first);
        let second = c_char_to_str(second);

        first.eq(second)
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_less_ignore_case(
        first: *const c_char,
        second: *const c_char,
    ) -> bool {
        let first = c_char_to_str(first);
        let second = c_char_to_str(second);

        less_ignore_case(first, second)
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_less_or_equal_ignore_case(
        first: *const c_char,
        second: *const c_char,
    ) -> bool {
        let first = c_char_to_str(first);
        let second = c_char_to_str(second);

        less_or_equal_ignore_case(first, second)
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_glob_ignore_case(
        s: *const c_char,
        pattern: *const c_char,
    ) -> bool {
        let s = c_char_to_str(s);
        let pattern = c_char_to_str(pattern);

        glob_ignore_case(s, pattern)
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_hash(s: *const c_char) -> uint64_t {
        let s = c_char_to_str(s);
        hash(s)
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_hash_ignore_case(s: *const c_char) -> uint64_t {
        let s = c_char_to_str(s);
        hash_ignore_case(s)
    }
}

#[cfg(test)]
mod tests {
    use std::{
        fs::File,
        io::{BufRead, BufReader},
    };

    use crate::string_utils::{
        ends_with_ignore_case, find_ignore_case, glob_ignore_case, hash, less_ignore_case,
        starts_with_ignore_case,
    };

    use super::hash_ignore_case;

    #[test]
    fn ignore_case_find_test() {
        let dataset = vec![
            ("GunPowder", "map_part", 0, -1),
            ("Bridgetown_Lightman", "_Lightman", 0, 10),
            ("banker_9_1", "banker", 0, 0),
            (
                "Sharle_1,Sharle_2,Sharle_3,Sharle_4,Sharle_5,protocusto,Sharle_6,Sharle_8",
                ",",
                0,
                8,
            ),
            (
                "Sharle_1,Sharle_2,Sharle_3,Sharle_4,Sharle_5,protocusto,Sharle_6,Sharle_8",
                ",",
                1,
                8,
            ),
            (
                "Sharle_1,Sharle_2,Sharle_3,Sharle_4,Sharle_5,protocusto,Sharle_6,Sharle_8",
                ",",
                9,
                17,
            ),
            (
                "Sharle_1,Sharle_2,Sharle_3,Sharle_4,Sharle_5,protocusto,Sharle_6,Sharle_8",
                ",",
                65,
                -1,
            ),
            ("Ромп,Ронгар,Рорда,Рос,Россау,Роттир", ",", 0, 8),
            ("Ромп,Ронгар,Рорда,Рос,Россау,Роттир", ",", 9, 21),
            ("Ромп,Ронгар,Рорда,Рос,Россау,Роттир", ",", 53, -1),
            ("Ромп,Ронгар,Рорда,Рос,Россау,Роттир", ",", 1, 8),
        ];

        for (s, pat, start, expected) in dataset {
            let result = find_ignore_case(s, pat, start);
            let expected = if expected == -1 {
                None
            } else {
                Some(expected as usize)
            };

            assert_eq!(result, expected);
        }
    }

    #[test]
    fn ignore_case_starts_with_test() {
        let datatest = vec![
            (r"ships\Fleut1\Fleut1", "mast", false),
            ("", "mast", false),
            ("mast4", "mast", true),
            ("editsection:nation", "editsection:", true),
            (
                "papirus_character_remove_officer,col:{255,128,128,128},pos:{190,190,610,360}",
                "editsection:",
                false,
            ),
        ];

        for (s, pat, expected) in datatest {
            let result = starts_with_ignore_case(s, pat);
            assert_eq!(result, expected);
        }
    }

    #[test]
    fn ignore_case_ends_with_test() {
        let datatest = vec![
            (r"WEATHER\SUN\GLOW\SUN.TGA.TX", ".tx", true),
            (r"WEATHER\SUN\GLOW\SUN.TGA.tx", ".tx", true),
            (r"WEATHER\SUN\GLOW\SUN.TGA.tx", ".TX", true),
            (r"WEATHER\SUN\GLOW\SUN.TGA.TX", ".TX", true),
            ("", ".tx", false),
            ("BLOT.TGA", ".tx", false),
        ];

        for (s, pat, expected) in datatest {
            let result = ends_with_ignore_case(s, pat);
            assert_eq!(result, expected);
        }
    }

    #[test]
    fn ignore_case_less_test() {
        let str_lowercase = "mystring";
        let str_uppercase = "MYSTRING";
        let str_mixedcase = "mYsTrInG";
        let str_long = "mystrings";

        // Identical string should not be less
        assert!(!less_ignore_case(str_lowercase, str_lowercase));
        assert!(!less_ignore_case(str_uppercase, str_uppercase));
        assert!(!less_ignore_case(str_mixedcase, str_mixedcase));

        // Lowercase should not be considered before uppercase
        assert!(!less_ignore_case(str_lowercase, str_uppercase));
        assert!(!less_ignore_case(str_uppercase, str_lowercase));
        assert!(!less_ignore_case(str_lowercase, str_mixedcase));
        assert!(!less_ignore_case(str_mixedcase, str_lowercase));
        assert!(!less_ignore_case(str_mixedcase, str_uppercase));
        assert!(!less_ignore_case(str_uppercase, str_mixedcase));

        // Characters that appear earlier in the alphabet should be cosidered less than character after it
        assert!(less_ignore_case("A", "b"));
        assert!(!less_ignore_case("c", "B"));

        // Shorter string should be considered before longer string, when they have the same starting sequence
        assert!(less_ignore_case(str_lowercase, str_long));
        assert!(!less_ignore_case(str_long, str_lowercase));
    }

    #[test]
    fn ignore_case_glob_test() {
        let datatest = vec![
            ("max.ini", "*.ini", true),
            ("max.ini", "*.INI", true),
            ("max.INI", "*.ini", true),
            ("AoP.prj", "*.xps", false),
            ("cancloud_coulverines.xps", "*.xps", true),
            ("0001_initial.c", "????_*.c", true),
            ("TestCase", "*", true),
        ];

        for (s, pat, expected) in datatest {
            let result = glob_ignore_case(s, pat);
            assert_eq!(result, expected);
        }
    }

    #[test]
    fn hash_test() {
        let s1 = "TEST";
        let s2 = "test";
        let s3 = "TeSt";

        let hash1 = hash(s1);
        let hash2 = hash(s2);
        let hash3 = hash(s3);

        assert_ne!(hash1, hash2);
        assert_ne!(hash2, hash3);
        assert_ne!(hash1, hash3);

        let hash1_2 = hash(s1);
        let hash2_2 = hash(s2);
        let hash3_2 = hash(s3);

        assert_eq!(hash1, hash1_2);
        assert_eq!(hash2, hash2_2);
        assert_eq!(hash3, hash3_2);
    }

    #[test]
    fn ignore_case_hash_test() {
        let s1 = "TEST";
        let s2 = "test";
        let s3 = "TeSt";

        let hash1 = hash_ignore_case(s1);
        let hash2 = hash_ignore_case(s2);
        let hash3 = hash_ignore_case(s3);

        assert_eq!(hash1, hash2);
        assert_eq!(hash2, hash3);
    }

    #[allow(dead_code)]
    fn for_each_line(file_name: &str, func: fn(&str)) -> Result<(), std::io::Error> {
        let file = File::open(&file_name)?;

        // uses a reader buffer
        let mut reader = BufReader::new(file);
        let mut line = String::new();

        loop {
            match reader.read_line(&mut line) {
                Ok(bytes_read) => {
                    // EOF: save last file address to restart from this address for next run
                    if bytes_read == 0 {
                        break;
                    }

                    func(line.trim());

                    // do not accumulate data
                    line.clear();
                }
                Err(err) => {
                    return Err(err);
                }
            };
        }

        Ok(())
    }
}
