use std::cmp::Ordering;

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

pub fn ignore_case_starts_with(s: &str, pat: &str) -> bool {
    let s_lowercase = s.to_lowercase();
    let pat_lowercase = pat.to_lowercase();
    s_lowercase.starts_with(&pat_lowercase)
}

fn ignore_case_compare(s1: &str, s2: &str) -> Ordering {
    let s1_lowercase = s1.to_lowercase();
    let s2_lowercase = s2.to_lowercase();

    s1_lowercase.cmp(&s2_lowercase)
}

pub fn ignore_case_equal(s1: &str, s2: &str) -> bool {
    matches!(ignore_case_compare(s1, s2), Ordering::Equal)
}

#[cfg(test)]
mod tests {
    use std::{
        fs::File,
        io::{BufRead, BufReader},
    };

    use crate::string_compare::{ignore_case_find, ignore_case_starts_with};

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
