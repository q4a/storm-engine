use std::cmp::Ordering;

pub fn ignore_case_find(s: &str, pattern: &str, start: usize) -> Option<usize> {
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

pub fn ignore_case_starts_with(s: &str, pattern: &str) -> bool {
    let s_lowercase = s.to_lowercase();
    let pattern_lowercase = pattern.to_lowercase();
    s_lowercase.starts_with(&pattern_lowercase)
}

fn ignore_case_compare(s1: &str, s2: &str) -> Ordering {
    let s1_lowercase = s1.to_lowercase();
    let s2_lowercase = s2.to_lowercase();

    s1_lowercase.cmp(&s2_lowercase)
}

pub fn ignore_case_equal(s1: &str, s2: &str) -> bool {
    matches!(ignore_case_compare(s1, s2), Ordering::Equal)
}

pub fn ignore_case_less(s1: &str, s2: &str) -> bool {
    matches!(ignore_case_compare(s1, s2), Ordering::Less)
}

pub fn ignore_case_less_or_equal(s1: &str, s2: &str) -> bool {
    !matches!(ignore_case_compare(s1, s2), Ordering::Greater)
}

pub fn ignore_case_glob(s: &str, pattern: &str) -> bool {
    let glob = globset::Glob::new(pattern).unwrap().compile_matcher();
    glob.is_match(s)
}

#[cfg(test)]
mod tests {
    use std::{
        fs::File,
        io::{BufRead, BufReader},
    };

    use crate::string_compare::{ignore_case_find, ignore_case_less, ignore_case_starts_with, ignore_case_glob};

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
            let result = ignore_case_find(s, pat, start);
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
            ("mast4", "mast", true),
            ("editsection:nation", "editsection:", true),
            (
                "papirus_character_remove_officer,col:{255,128,128,128},pos:{190,190,610,360}",
                "editsection:",
                false,
            ),
        ];

        for (s, pat, expected) in datatest {
            let result = ignore_case_starts_with(s, pat);
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

    #[test]
    fn ignore_case_glob_test() {
        for_each_line("test_data.txt", |line| {
            let splits = line.split("|").collect::<Vec<_>>();
            assert_eq!(splits.len(), 3);

            let pat = splits[0];
            let path = splits[1];
            let expected = str::parse::<i8>(splits[2]).unwrap();
            let expected = match expected {
                0 => false,
                1 => true,
                _ => panic!("Can't convert the value")
            };

            let result = ignore_case_glob(path, pat);
            assert_eq!(result, expected);
        }).unwrap();
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
