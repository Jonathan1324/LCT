use std::ffi::{CStr, CString};
use std::io::{Write, Read};
use std::process::{Command, Stdio};
use std::os::raw::c_char;
use std::thread;

// TODO: I don't like that it uses threads

#[no_mangle]
pub extern "C" fn run_program(
    cmd: *const c_char,
    input: *const c_char,
    out_buf: *mut *mut c_char,
    err_buf: *mut *mut c_char,
) -> i32 {
    unsafe {
        let c_cmd = CStr::from_ptr(cmd).to_string_lossy();
        let c_input = CStr::from_ptr(input).to_string_lossy();
        let input_bytes = c_input.as_bytes().to_vec();

        let mut parts = c_cmd.split_whitespace();
        let program = match parts.next() { Some(p) => p, None => return -1 };
        let args: Vec<&str> = parts.collect();

        let mut child = match Command::new(program)
            .args(&args)
            .stdin(Stdio::piped())
            .stdout(Stdio::piped())
            .stderr(Stdio::piped())
            .spawn()
        { Ok(c) => c, Err(_) => return -1 };

        let mut stdin = child.stdin.take();
        let stdin_thread = thread::spawn(move || {
            if let Some(mut s) = stdin {
                let mut pos = 0;
                while pos < input_bytes.len() {
                    let end = std::cmp::min(pos + 8192, input_bytes.len());
                    if s.write_all(&input_bytes[pos..end]).is_err() {
                        break;
                    }
                    pos = end;
                }
                let _ = s.flush();
            }
        });

        let mut stdout = child.stdout.take().unwrap();
        let stdout_thread = thread::spawn(move || {
            let mut buf = Vec::new();
            let _ = stdout.read_to_end(&mut buf);
            buf
        });

        let mut stderr = child.stderr.take().unwrap();
        let stderr_thread = thread::spawn(move || {
            let mut buf = Vec::new();
            let _ = stderr.read_to_end(&mut buf);
            buf
        });

        let status = match child.wait() {
            Ok(s) => s,
            Err(_) => return -1,
        };

        let stdout_buf = stdout_thread.join().unwrap_or_default();
        let stderr_buf = stderr_thread.join().unwrap_or_default();
        let _ = stdin_thread.join();

        *out_buf = CString::new(stdout_buf).unwrap_or_else(|_| CString::new("").unwrap()).into_raw();
        *err_buf = CString::new(stderr_buf).unwrap_or_else(|_| CString::new("").unwrap()).into_raw();

        status.code().unwrap_or(-1)
    }
}

#[no_mangle]
pub extern "C" fn free_c_string(s: *mut c_char) {
    if !s.is_null() {
        unsafe { CString::from_raw(s); }
    }
}
