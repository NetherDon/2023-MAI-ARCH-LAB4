CREATE TABLE IF NOT EXISTS `User` 
(
    `id` INT NOT NULL AUTO_INCREMENT,
    `first_name` VARCHAR(256) NOT NULL,
    `last_name` VARCHAR(256) NOT NULL,
    `login` VARCHAR(256) NOT NULL UNIQUE,
    `password` VARCHAR(256) NOT NULL,
    `email` VARCHAR(256) NULL,
    `title` VARCHAR(1024) NULL,

    PRIMARY KEY (`id`),
    KEY `fn` (`first_name`),
    KEY `ln` (`last_name`)
);

CREATE TABLE IF NOT EXISTS `Product` 
(
    `id` INT NOT NULL AUTO_INCREMENT,
    `name` VARCHAR(256) NOT NULL,
    `type` VARCHAR(256) NOT NULL,
    `description` VARCHAR(1024) NULL,
    `manufactorer` VARCHAR(256) NOT NULL,
    `price` INT NOT NULL,

    PRIMARY KEY (`id`),
    KEY `n` (`name`)
);

CREATE TABLE IF NOT EXISTS `Cart` 
(
    `id` INT NOT NULL AUTO_INCREMENT,
    `productId` INT NOT NULL,
    `userLogin` VARCHAR(256) NOT NULL,

    PRIMARY KEY (`id`),
    FOREIGN KEY (`productId`) REFERENCES Product (`id`),
    KEY `ul` (`userLogin`)
);

INSERT INTO `User` (`first_name`, `last_name`, `email`, `title`, `login`, `password`) VALUE
("Artem", "Chirkov", "my@mail.ru", "Mister", "log", "123"); -- sharding:1

INSERT INTO `User` (`first_name`, `last_name`, `email`, `title`, `login`, `password`) VALUE
("Ivan", "Konstantinov", "random@gmail.com", "Mister", "apple", "122333"); -- sharding:0

INSERT INTO `User` (`first_name`, `last_name`, `email`, `title`, `login`, `password`) VALUE
("Valery", "Popov", "valp@yandex.ru", "Mister", "test", "pass2"); -- sharding:1

INSERT INTO `Product` (`name`, `type`, `manufactorer`, `description`, `price`) VALUES
("Apple", "Fruit", "Farm Frenzy", "", 10),
("Orange", "Fruit", "Farm Frenzy", "", 12),
("Carrot", "Vegetable", "Farm Frenzy", "Healthy food", 9),
("Milk", "Drink", "Farm Frenzy", "", 21),
("Xperia Z", "Phone", "Sony", "", 10000),
("IPhone", "Phone", "Apple", "Powerful and expensive phone", 90000),
("Nvidia GeForce RTX 4090", "Video Card", "Nvidia", "", 160000);

INSERT INTO `Cart` (`userId`, `productId`) VALUES
(1, 3),
(3, 4),
(3, 5);