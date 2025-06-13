## place holders for event















#Event Manager
## Design concept

## Functionality

## WIP

## 🪵 Logging Level Support (Nice-to-Have Feature)

To improve debugging and observability across engine systems, we recommend introducing a **log level system** that allows selective output of messages. This helps separate normal information from warnings or critical failures.

### Suggested Log Levels

| Level  | Description                                                 |
|--------|-------------------------------------------------------------|
| DEBUG  | Verbose output for internal diagnostics, e.g., "Event queued: KEY_DOWN" |
| INFO   | Standard runtime flow, e.g., "Scene loaded: Level2"         |
| WARN   | Unexpected but non-fatal issues, e.g., "No listeners found for SCENE_CHANGE" |
| ERROR  | Critical or recoverable failures, e.g., "Listener threw exception during dispatch" |

### Example Output
[INFO] Registered listener for KEY_DOWN
[DEBUG] Queued event: KEY_DOWN
[ERROR] Listener threw exception: std::bad_cast


### Future Implementation Ideas

- Add a `Logger` utility with `log(Level, message)` interface  
- Allow filtering log levels at runtime (e.g., via config or CLI flags)  
- Optionally write logs to file for persistent debugging and post-mortem analysis  
