#pragma once

// An enum used to identify the type of key a Key object represents.
enum KeyType
{
    // Key objects of this type have been initialized as neither an HEKey or DigitalKey object.
    Base,

    // A Key object that was initialized as an HEKey object.
    HallEffect,

    // A Key object that was initialized as a DigitalKey object.
    Digital
};
