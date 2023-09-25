CREATE TABLE IF NOT EXISTS flight (
    carrier TEXT,
    flightNo INT,
    departureDatetime TEXT,
    arrivalDatetime TEXT,
    departure TEXT,
    arrival TEXT
);

CREATE INDEX IF NOT EXISTS flight_index ON flight (
    carrier, flightNo, departure, arrival
);


CREATE TABLE IF NOT EXISTS price (
    carrier TEXT,
    departure TEXT,
    arrival TEXT,
    cabin TEXT,
    amount INT,
    CHECK(cabin IN ('F', 'C', 'Y'))
);

CREATE INDEX IF NOT EXISTS flight_index ON flight (
    carrier, departure, arrival
);

CREATE TABLE IF NOT EXISTS price_rule (
    sequenceNo INT UNIQUE,
    carrier TEXT,
    departure TEXT,
    arrival TEXT,
    nextCarrier TEXT,
    agencies TEXT,
    subcharge INT,
    CHECK(subcharge >= -1 AND subcharge <= 100)
);

CREATE TABLE IF NOT EXISTS seat (
    carrier TEXT,
    flightNo INT,
    departure TEXT,
    arrival TEXT,
    departureDatetime TEXT,
    seatF,
    seatC,
    seatY,
    FOREIGN KEY (carrier, flightNo, departure, arrival, departureDatetime) REFERENCES flight(carrier, flightNo, departure, arrival, departureDatetime)
);

CREATE INDEX IF NOT EXISTS seat_index ON seat (
    carrier, flightNo, departure, arrival
);
