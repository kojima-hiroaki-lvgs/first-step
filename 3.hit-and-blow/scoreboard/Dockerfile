FROM node:22-bookworm
LABEL authors="shu.nonaka"

ARG TINIVERSION=v0.19.0
ARG TARGETARCH

ENV DATABASE_URL="file:db/scoreboard.db"

WORKDIR /app

ADD https://github.com/krallin/tini/releases/download/${TINIVERSION}/tini-${TARGETARCH} /tini
RUN chmod +x /tini

COPY package*.json .
RUN npm ci

COPY prisma prisma
RUN npx prisma generate

COPY daemon.js scoreboard.tsx tsconfig.json ./

ENTRYPOINT ["/tini", "--"]


