/*
  Warnings:

  - Added the required column `digitsLength` to the `Scores` table without a default value. This is not possible if the table is not empty.

*/
-- RedefineTables
PRAGMA defer_foreign_keys=ON;
PRAGMA foreign_keys=OFF;
CREATE TABLE "new_Scores" (
    "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    "name" TEXT NOT NULL,
    "digitsLength" INTEGER NOT NULL,
    "attempts" INTEGER NOT NULL,
    "startAt" DATETIME NOT NULL,
    "endAt" DATETIME NOT NULL
);
INSERT INTO "new_Scores" ("attempts", "endAt", "id", "name", "startAt") SELECT "attempts", "endAt", "id", "name", "startAt" FROM "Scores";
DROP TABLE "Scores";
ALTER TABLE "new_Scores" RENAME TO "Scores";
PRAGMA foreign_keys=ON;
PRAGMA defer_foreign_keys=OFF;
