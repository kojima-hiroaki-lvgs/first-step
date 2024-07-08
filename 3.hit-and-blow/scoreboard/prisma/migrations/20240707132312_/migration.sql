-- CreateTable
CREATE TABLE "Scores" (
    "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    "name" TEXT NOT NULL,
    "attempts" INTEGER NOT NULL,
    "startAt" DATETIME NOT NULL,
    "endAt" DATETIME NOT NULL
);
