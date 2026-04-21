

-- Select all users from the database
SELECT * FROM Users

-- Create table for users
CREATE TABLE "Users"
(
    "ID" INTEGER NOT NULL UNIQUE, "DiscordUsername" TEXT NOT NULL, "DiscordID" NUMERIC NOT NULL, "FractionID" INTEGER,
    PRIMARY KEY("ID" AUTOINCREMENT), FOREIGN KEY("FractionID") REFERENCES "Fractions"("ID")
);

-- Create table for fractions
CREATE TABLE "Fractions"
(
    "ID" INTEGER NOT NULL UNIQUE, "Name" TEXT, "Description" TEXT, "CurrentExp" NUMERIC, "ExpToNextLevel" NUMERIC, "Level" INTEGER,
    PRIMARY KEY("ID" AUTOINCREMENT)
);