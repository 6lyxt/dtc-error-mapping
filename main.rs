use std::error::Error;
use std::fs::File;
use std::io::{BufRead, BufReader};

const JSON_FILE: &str = "dtc.json";
const MAX_LINE_LENGTH: usize = 256;

fn main() -> Result<(), Box<dyn Error>> {
    let file = File::open("data.txt")?;
    let reader = BufReader::new(file);

    for line in reader.lines() {
        let line = line?;
        if let Some(error_start) = line.find("[") {
            let error_end = line.find("]").unwrap_or(line.len());
            let error_codes = &line[(error_start + 1)..error_end];
            let error_codes: Vec<&str> = error_codes.split(", ").collect();

            if !error_codes.is_empty() {
                let mut found_error = false;

                for error_code in error_codes {
                    if let Some(message) = get_error_message(error_code.trim())? {
                        println!("Error Code {}: {}", error_code, message);
                        found_error = true;
                    } else {
                        println!("Error Code {}: Unknown error", error_code);
                    }
                }

                if !found_error {
                    println!("No error codes found");
                }
            }
        }
    }

    Ok(())
}

fn get_error_message(error_code: &str) -> Result<Option<String>, Box<dyn Error>> {
    let file = File::open(JSON_FILE)?;
    let reader = BufReader::new(file);

    let mut code_found = false;
    let mut message_found = false;
    let mut error_message = String::new();

    for line in reader.lines() {
        let line = line?;

        if line.contains(&format!("\"Code\": \"{}\"", error_code)) {
            code_found = true;
        }

        if code_found && line.contains("\"Message\":") {
            let message_start = line.find("\"Message\":").unwrap() + 11;
            let message_end = line.rfind("\"").unwrap();
            let message = &line[message_start..message_end];
            error_message = message.to_string();
            message_found = true;
            break;
        }
    }

    if code_found && message_found {
        Ok(Some(error_message))
    } else {
        Ok(None)
    }
}
