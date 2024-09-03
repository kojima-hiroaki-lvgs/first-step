const net = require("net")

const conn = net.createConnection(9600, "111.237.115.70");

conn.on("connect", () => conn.write(Buffer.from([0, 3, 0x41, 0x41, 0x42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])));
conn.on("data", data => {
    console.log(data);

    if (data[0] === 3 && data[1] === 0) {
        conn.end();
        return
    }

    conn.write(Buffer.from([2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]));
})