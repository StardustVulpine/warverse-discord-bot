-- Create table for users
CREATE TABLE "Users"
(
    "DiscordID" NUMERIC NOT NULL UNIQUE,
    "DiscordUsername" TEXT NOT NULL,
    "FractionID" INTEGER,
    PRIMARY KEY("DiscordID"),
    FOREIGN KEY("FractionID") REFERENCES "Fractions"("DiscordRoleID")
);

-- Create table for fractions
CREATE TABLE "Fractions"
(
    "DiscordRoleID" NUMERIC NOT NULL UNIQUE,
    "Name" TEXT,
    "Description" TEXT,
    "Level" INTEGER DEFAULT 0,
    "CurrentExp" NUMERIC DEFAULT 0,
    "ExpToNextLevel" NUMERIC DEFAULT 1000,
    "NextLevelExpRequirement_Mult" FLOAT DEFAULT 1.5,
    PRIMARY KEY ("DiscordRoleID")
);

-- Insert new user into Users table
INSERT INTO Users (DiscordUsername, DiscordID) VALUES ('', '')

-- Get fraction name by its ID
SELECT Name FROM Fractions WHERE ID = '{}'

DELETE FROM Users *