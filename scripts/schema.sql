CREATE TABLE IF NOT EXISTS flight (
    carrier TEXT NOT NULL,
    flightNo INT NOT NULL,
    departureDatetime TEXT NOT NULL,
    arrivalDatetime TEXT NOT NULL,
    departure TEXT NOT NULL,
    arrival TEXT NOT NULL,
    PRIMARY KEY (carrier, flightNo, departure, arrival, departureDatetime)
);

CREATE INDEX IF NOT EXISTS flight_index ON flight (
    carrier, flightNo, departure, arrival
);

CREATE TABLE IF NOT EXISTS price (
    carrier TEXT NOT NULL,
    departure TEXT NOT NULL,
    arrival TEXT NOT NULL,
    cabin TEXT NOT NULL,
    amount INT NOT NULL,
    CHECK(cabin IN ('F', 'C', 'Y'))
);

CREATE INDEX IF NOT EXISTS price_index ON price (
    carrier, departure, arrival
);

CREATE TABLE IF NOT EXISTS price_rule (
    sequenceNo INT UNIQUE NOT NULL,
    carrier TEXT NOT NULL,
    departure TEXT,
    arrival TEXT,
    nextCarrier TEXT,
    agencies TEXT,
    subcharge INT NOT NULL,
    CHECK(subcharge >= -1 AND subcharge <= 100)
);

CREATE TABLE IF NOT EXISTS seat (
    carrier TEXT NOT NULL,
    flightNo INT NOT NULL,
    departure TEXT NOT NULL,
    arrival TEXT NOT NULL,
    departureDatetime TEXT NOT NULL,
    seatF TEXT NOT NULL,
    seatC TEXT NOT NULL,
    seatY TEXT NOT NULL,
    FOREIGN KEY (carrier, flightNo, departure, arrival, departureDatetime) REFERENCES flight(carrier, flightNo, departure, arrival, departureDatetime),
    PRIMARY KEY (carrier, flightNo, departure, arrival, departureDatetime)
);

CREATE INDEX IF NOT EXISTS seat_index ON seat (
    carrier, flightNo, departure, arrival
);