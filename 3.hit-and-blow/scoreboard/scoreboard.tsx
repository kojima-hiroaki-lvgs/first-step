import {serve} from '@hono/node-server'
import {Hono} from 'hono'
import {PrismaClient, Scores} from "@prisma/client";

const app = new Hono()
const prisma = new PrismaClient();

app.get('/', async (c) => {
    const digits = c.req.query("digits") ?? 3;
    const scores = await prisma.$queryRaw<Scores[]>`
        SELECT *
        FROM Scores
        WHERE digitsLength = ${digits}
        ORDER BY attempts ASC, endAt - startAt ASC LIMIT 20
    `;

    return c.html(
        <html lang="ja">
        <head>
        </head>
        <body>
        <main>
            <h1>Hit and Blow スコアボード</h1>
            <nav>
                <span style="margin: 0 8px 0 8px"><a href="?digits=3">3</a></span>
                <span style="margin: 0 8px 0 8px"><a href="?digits=4">4</a></span>
                <span style="margin: 0 8px 0 8px"><a href="?digits=5">5</a></span>
                <span style="margin: 0 8px 0 8px"><a href="?digits=6">6</a></span>
                <span style="margin: 0 8px 0 8px"><a href="?digits=7">7</a></span>
                <span style="margin: 0 8px 0 8px"><a href="?digits=8">8</a></span>
                <span style="margin: 0 8px 0 8px"><a href="?digits=9">9</a></span>
            </nav>
            <table>
                <thead>
                <tr>
                    <th>名前</th>
                    <th>ケタ数</th>
                    <th>試行回数</th>
                    <th>思考時間</th>
                </tr>
                </thead>
                <tbody>
                {scores.map(s => (
                    <tr>
                        <td>{s.name}</td>
                        <td>{s.digitsLength}</td>
                        <td>{s.attempts}回</td>
                        <td>{((s.endAt.getTime() - s.startAt.getTime()) / 1000)}秒</td>
                    </tr>
                ))}
                </tbody>
            </table>
        </main>
        </body>
        </html>
    )
})

const port = 3000
console.log(`Server is running on port ${port}`)

serve({
    fetch: app.fetch,
    port
})
