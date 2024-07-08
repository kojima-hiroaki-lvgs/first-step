import {serve} from '@hono/node-server'
import {Hono} from 'hono'
import {PrismaClient, Scores} from "@prisma/client";

const app = new Hono()
const prisma = new PrismaClient();

app.get('/', async (c) => {
    const scores = await prisma.$queryRaw<Scores[]>`
        SELECT *
        FROM Scores
        ORDER BY attempts ASC, endAt - startAt ASC LIMIT 20
    `;

    return c.html(
        <html lang="ja">
        <head>
        </head>
        <body>
        <main>
            <h1>Hit and Blow スコアボード</h1>
            <table>
                <thead>
                <tr>
                    <th>名前</th>
                    <th>試行回数</th>
                    <th>思考時間</th>
                </tr>
                </thead>
                <tbody>
                {scores.map(s => (
                    <tr>
                        <td>{s.name}</td>
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
