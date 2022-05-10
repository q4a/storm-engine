use std::{collections::HashMap, ffi::OsString};

use encoding_rs::{Encoding, WINDOWS_1251};
use once_cell::sync::Lazy;

/// Patches that should be applied to files. Currently it's a static inmemory variable,
/// but in the future should probably load it from the file instead
pub static CHANGE_SET: Lazy<ChangeSet> = Lazy::new(|| {
    let patches = vec![
        (
            "crabSmall.ani",
            15393,
            vec![Patch::Decode {
                encoding: WINDOWS_1251,
            }],
        ),
        (
            "man_B.ani",
            40353,
            vec![Patch::Decode {
                encoding: WINDOWS_1251,
            }],
        ),
        (
            "interfaces.ini",
            838,
            vec![Patch::Replace {
                start: 105,
                end: 106,
                replacement: ";".as_bytes().to_vec(),
            }],
        ),
    ];

    let mut change_set = ChangeSet {
        patches: HashMap::new(),
    };

    for (file_name, file_size, changes) in patches {
        let file_id = FileID {
            file_name: OsString::from(file_name),
            file_size,
        };
        change_set.patches.insert(file_id, changes);
    }

    change_set
});

/// Represents one single modification applied to a file
pub enum Patch {
    Decode {
        encoding: &'static Encoding,
    },
    Replace {
        start: usize,
        end: usize,
        replacement: Vec<u8>,
    },
}

/// Stores metadata to detect files that need to be patched.
/// Currently uses `file_name` and `file_size`, but should probably replace `file_size` with a hashsum
#[derive(Hash, PartialEq, Eq)]
pub struct FileID {
    pub file_name: OsString,
    pub file_size: u64,
}

/// Stores all patches for all files. In the future should probably replace `FileID` with just `file_name` as a key
/// and use a hashsum of the file to detect various versions
pub struct ChangeSet {
    pub patches: HashMap<FileID, Vec<Patch>>,
}
