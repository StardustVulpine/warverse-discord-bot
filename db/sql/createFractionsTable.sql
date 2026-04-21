CREATE TABLE "Fractions"
(
    "ID" INTEGER NOT NULL UNIQUE, "Name" TEXT, "Description" TEXT, "CurrentExp" NUMERIC, "ExpToNextLevel" NUMERIC, "Level" INTEGER,
    PRIMARY KEY("ID" AUTOINCREMENT)
);