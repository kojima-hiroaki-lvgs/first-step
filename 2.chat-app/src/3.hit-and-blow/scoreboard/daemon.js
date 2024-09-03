const net = require("net");
const {PrismaClient} = require("@prisma/client")

///                 NumeronProtocol Packet Format
///
///                      Negotiation Request
/// 0                   1                   2                   3
/// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// | | |           name (alphabet or numeric only)               |
/// +|+|+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///  | |
///  | number of digits (3 ~ 9)
///  |
///  packet type
///    0: Negotiation Request
///    1: Negotiation Response
///    2: Answer Request
///    3: Answer Response
///
///                      Negotiation Response
/// 0                   1                   2                   3
/// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// | | |                          padding                        |
/// +|+|+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///  | |
///  | response status
///  |   0: Success
///  |   1: Invalid digits
///  |   2: Invalid name
///  packet type
///
///                         Answer Request
/// 0                   1                   2                   3
/// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// | |                 |      answer       |       padding       |
/// +|+-+-+-+|+-+-+-+-+-+-+-+|+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///  |       |               |
///  |       |               digits as integer
///  |       |                 example, when you have guessed answer as 34865, this segment will be [0x03, 0x04, 0x08, 0x06, 0x05, 0, 0, 0, 0, 0]
///  |       reserved (filled with zero)
///  packet type
///
///                        Answer Response
/// 0                   1                   2                   3
/// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
/// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// | | |               |   |                                     |
/// +|+|+-+-+-+|+-+-+-+-+-+|+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///  | |       |           |
///  | |       |           hint
///  | |       |             first byte is Hit, second byte is Blow.
///  | |       reserved (filled with zero)
///  | response status
///  |   0: Correct
///  |   1: Hint
///  |   2: Invalid request
///  |   3: Invalid digit
///  |
///  packet type

const prisma = new PrismaClient();

const server = net.createServer(socket => {
    let name;
    let numbers = [1, 2, 3];
    let startAt;
    let attempts = 0;

    socket.on("data", (data) => {
        if (data[0] === 0) {
            socket.emit("negotiate", data);
            return;
        }

        if (data[0] === 2) {
            socket.emit("answer", data);
            return;
        }

        socket.end();
    })

    socket.on("negotiate", handleNegotiate);
    socket.on("answer", handleAnswer);

    function handleNegotiate(data) {
        const numberOfDigits = data[1];

        if (numberOfDigits < 3 || numberOfDigits > 9) {
            socket.write(Buffer.from([1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]));
            socket.end();
            return;
        }

        name = data.slice(2, data.indexOf(0, 2)).toString("ascii").trim();

        if (name.length < 3) {
            socket.write(Buffer.from([1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]));
            socket.end();
            return;
        }

        const numberPool = new Set([0, 1, 2, 3, 4, 5, 6, 7, 8, 9]);

        // while (numbers.length < numberOfDigits) {
        //     const i = Math.round(Math.random() * 1000) % numberPool.size;
        //     const n = [...numberPool][i];
        //     numbers.push(n);
        //     numberPool.delete(n);
        // }

        socket.write(Buffer.from([1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]));
        startAt = new Date();
    }

    async function handleAnswer(data) {
        if (numbers.length === 0) {
            socket.emit("error");
        }

        const answer = data.slice(10, 10 + numbers.length);

        if (!answer.every(c => 0 <= c && c <= 9)) {
            socket.write(Buffer.from([3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]));
        }

        ++attempts;
        let hit = 0, blow = 0;

        answer.forEach((digit, i) => {
            if (digit === numbers[i]) {
                hit++;
            } else if (numbers.includes(digit)) {
                blow++;
            }
        })

        if (hit === numbers.length) {
            socket.write(Buffer.from([3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]));
            socket.end();
            const endAt = new Date();

            await prisma.scores.create({
                data: {
                    name,
                    attempts,
                    startAt,
                    endAt
                }
            });
        } else {
            socket.write(Buffer.from([3, 1, 0, 0, 0, 0, 0, 0, 0, 0, hit, blow, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]))
        }
    }
});

server.listen(9600, "0.0.0.0", () => {
    console.log("server listening on port 9600!");
});