# **Admin Server - Specification**

---

## **1. Overview**

The **Admin Server** is the central controller for the entire water treatment system. It:

- **Manages water flow rates** for all connected servers.
- **Triggers environmental events** that affect the system (e.g., chemical spills, infrastructure decay).
- **Monitors server performance** and logs key metrics.
- **Ensures data integrity** by coordinating water movement through the network.

The Admin Server **does not process water directly** but serves as a control hub for all other servers.

---

## **2. Functional Requirements**

### **2.1 General Requirements**

1. **Written in C**.
2. **Runs as a TCP server**, listening for connections from other servers.
3. **Receives periodic status updates** from all water treatment servers.
4. **Sends control commands** to connected servers to adjust flow rates.
5. **Triggers environmental events** to simulate real-world conditions.
6. **Logs all events, commands, and server statistics**.

### **2.2 Water Flow Control**

- The **Admin Server** must manage the flow of water between servers by sending **rate adjustment commands**.
- Each server **reports its current flow rate** every `X` seconds.
- The Admin Server **decides** whether to **increase, decrease, or block flow** based on predefined rules.

**Example Control Commands:**

| **Command**        | **Description**                         | **Target** |
|--------------------|-------------------------------------|------------|
| `SET_FLOW <server> <rate>` | Adjusts flow rate for a server | Any server |
| `BLOCK_FLOW <server>`  | Temporarily halts all water flow | Any server |
| `RESUME_FLOW <server>` | Resumes normal water flow | Any server |
| `DUMP_STATS` | Requests a full status report | Any server |

---

### **2.3 Environmental Event System**

The **Admin Server** must be able to inject **random or manual environmental events** that impact the system.

**Example Events:**

| **Event Type**         | **Effect** |
|------------------------|-----------|
| **Chemical Spill**     | Contaminates water with toxic elements. |
| **Illegal Dumping**    | Introduces unexpected solid waste. |
| **Torrential Rain**    | Overwhelms the storm drain, increasing flow rates. |
| **Drought**           | Reduces incoming water. |
| **Infrastructure Failure** | Causes temporary shutdown of a server. |

- Events can be **triggered manually** by an admin or **randomly** based on a timer.
- When an event occurs, the **Admin Server broadcasts** it to all affected servers.
- Servers must **adjust their behavior** accordingly.

---

### **2.4 Monitoring and Logging**

- The **Admin Server must log** all events, commands, and errors.
- Logs should include **timestamps, affected servers, and relevant parameters**.
- It must provide **real-time status reports** for debugging and performance evaluation.

**Example Log Entries:**

```yaml
[2025-02-06 12:30:00] EVENT: Chemical Spill at Pretreatment Server
[2025-02-06 12:31:00] COMMAND: SET_FLOW residential 75%
[2025-02-06 12:35:00] ERROR: Storm Drain Server Unresponsive
```

---

## **3. Communication Protocol**

The **Admin Server** must support **bidirectional TCP communication** with all water treatment servers.

### **3.1 Server-Admin Messages**

Each water treatment server must send periodic status updates to the **Admin Server**.

| **Message Type** | **Format** | **Example** |
|------------------|-----------|-------------|
| STATUS_UPDATE | `STATUS <server> <flow_rate> <error_code>` | `STATUS residential 85% OK` |
| REQUEST_COMMAND | `REQUEST <server> <issue>` | `REQUEST pretreatment low_water` |

### **3.2 Admin-Server Commands**

The Admin Server can issue **control commands** to individual servers.

| **Command** | **Format** | **Example** |
|------------|-----------|-------------|
| SET_FLOW | `SET_FLOW <server> <rate>` | `SET_FLOW stormdrain 90%` |
| BLOCK_FLOW | `BLOCK_FLOW <server>` | `BLOCK_FLOW treatment` |
| RESUME_FLOW | `RESUME_FLOW <server>` | `RESUME_FLOW treatment` |
| TRIGGER_EVENT | `TRIGGER_EVENT <event>` | `TRIGGER_EVENT chemical_spill` |

- Messages are exchanged over **TCP sockets**.
- All messages must be **ACKed** (acknowledged) by the receiving server.

---

## **4. Error Handling**

The Admin Server must gracefully handle:

- **Lost connections** (server crashes or disconnects).
- **Malformed messages** (invalid formats).
- **Unresponsive servers** (timeouts).
- **Conflicting flow adjustments** (ensuring no two commands contradict).

For each issue, an appropriate log entry must be generated.

---

## **5. Security Considerations**

1. **Prevent unauthorized commands** – Only authorized clients (water treatment servers) can issue requests.
2. **Implement authentication** – Servers must use a shared secret or key for command validation.
3. **Limit event frequency** – Prevent excessive triggering of disruptive events.
4. **Rate-limit connections** – Avoid overloading the admin server with requests.

---

## **6. Compilation & Execution**

### **6.1 Compilation**

The **Makefile** should support both **32-bit and 64-bit** builds.

```bash
# Build 64-bit version
make

# Build 32-bit version
make ARCH=32
```

The compiled binary should be named **`admin_server`**.

### **6.2 Running the Admin Server**

The server should start listening for connections:

```bash
./admin_server --port 8080
```

To trigger a manual event:

```bash
echo "TRIGGER_EVENT chemical_spill" | nc localhost 8080
```

---

## **7. Deliverables**

1. **Source code in C** implementing the Admin Server.
2. **Makefile** supporting both **32-bit and 64-bit** compilation.
3. **Logging system** for tracking all commands, events, and errors.
4. **Network protocol documentation** detailing message formats.
5. **README** with usage instructions and setup guide.

---

## Packet Structures

### **Next Steps**

- **Review this spec** and make any necessary adjustments.
- **Create a grading rubric** for evaluating the Admin Server.
- **Start planning the Storm Drain Server next.**
