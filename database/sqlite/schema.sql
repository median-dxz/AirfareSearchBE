CREATE TABLE IF NOT EXISTS flight (
    carrier CHAR(2),
    flightNo NUMERIC,
    departureDatetime CHAR(12),
    arrivalDatetime CHAR(12),
    departure CHAR(3),
    arrival CHAR(3)
);

CREATE INDEX IF NOT EXISTS flight_index ON flight (
    carrier, flightNo, departure, arrival
);


CREATE TABLE IF NOT EXISTS price (
    carrier CHAR(2),
    departure CHAR(3),
    arrival CHAR(3),
    cabin CHAR(1),
    amount NUMERIC,
    CHECK(cabin IN ('F', 'C', 'Y'))
);

CREATE INDEX IF NOT EXISTS flight_index ON flight (
    carrier, departure, arrival
);

CREATE TABLE IF NOT EXISTS price_rule (
    sequenceNo NUMERIC UNIQUE,
    carrier CHAR(2),
    departure CHAR(3),
    arrival CHAR(3),
    nextCarrier CHAR(2),
    agencies TEXT,
    subcharge NUMERIC,
    CHECK(subcharge >= -1 AND subcharge <= 100)
);

CREATE TABLE IF NOT EXISTS seat (
    carrier CHAR(2),
    flightNo NUMERIC,
    departure CHAR(3),
    arrival CHAR(3),
    departureDatetime CHAR(12),
    seatF,
    seatC,
    seatY,
    FOREIGN KEY (carrier, flightNo, departure, arrival, departureDatetime) REFERENCES flight(carrier, flightNo, departure, arrival, departureDatetime)
);

CREATE INDEX IF NOT EXISTS seat_index ON seat (
    carrier, flightNo, departure, arrival
);
