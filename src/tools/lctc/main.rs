use std::fs::File;
use std::io::{self, BufRead, BufReader};
use std::env;
use std::process;

pub mod c;

fn print_help() {

}

fn main() {
    for arg in env::args().skip(1) {
        match arg.as_str() {
            "-v" | "--version" => {
                unsafe { c::printVersion(); }
                process::exit(0);
            }
            "-h" | "--help" => {
                print_help();
                process::exit(0);
            }
            _ => {},
        }
    }
}
