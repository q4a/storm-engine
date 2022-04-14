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
}
