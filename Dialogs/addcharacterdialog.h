#ifndef ADDCHARACTERDIALOG_H
#define ADDCHARACTERDIALOG_H

#include <QDialog>

namespace Ui
{
    class AddCharacterDialog;
}

class AddCharacterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddCharacterDialog(QWidget *parent = nullptr);
    ~AddCharacterDialog();

public:
    //getters for the configured data
    QString getCharName()   { return m_charName; }
    QString getBio()        { return m_charBio; }
    QString getImagePath()  { return m_imagePath; }

private slots:
    void OnCharNameChanged();
    void OnCharBioChanged();
    void OnCharImageClicked();
    void OnCharImageDropped(const QString& path);
private:
    void SetPixmap(QPixmap& pix, const QString& path);
    void validate();

private:
    std::unique_ptr<Ui::AddCharacterDialog> ui;

    QString m_charName;   //name of the character
    QString m_charBio;    //description of the character
    QString m_imagePath;  //path to the image for the character (to be copied)
};

#endif // ADDCHARACTERDIALOG_H



/*
Problem Description
You're creating a website where people can track the bus and figure out when to go to the station. The buses move between the stations in one direction. The goal is to find the station any bus is currently at or find how many stops away the nearest bus is to a requested station. Buses are never between stations, they are only stopped at a station.

class BusTracker {
    // station list is the list of stations delimited by a hyphen (-)
    //   e.g. the station list can look like 1-2-3-4-5 and this will
    //   correspond to a map that looks like 1 -> 2 -> 3 -> 4 -> 5
    //   because buses only move in one direction, from left to right
    // busLocations is the current list of buses and the stations they are
    //   currently at
    // these two fields provide the input to figure out the next two functions
    constructor(stationList, busLocations) {}

    // given the buses flow in one direction, find how many stops away
    // the next bus is to the provided station
    public nearestBusToStation(stationId: string): number {
        return -1;
    }

    // given a bus id, return the station the bus is currently at
    public getBusLocation(busId: number): string {
        return "";
    };
}

For station map: "A-B-C-D-E-F-G-H-I"
And bus positions: { 1: "B", 2: "E" }

busTracker.nearestBusToStation("A") // -1
busTracker.nearestBusToStation("B") // 0
busTracker.nearestBusToStation("C") // 1
busTracker.nearestBusToStation("D") // 2
busTracker.nearestBusToStation("E") // 0
busTracker.nearestBusToStation("F") // 1
busTracker.nearestBusToStation("G") // 2
busTracker.nearestBusToStation("H") // 3
busTracker.nearestBusToStation("I") // 4
busTracker.getBusLocation(1); // B
busTracker.getBusLocation(2); // E

*/
/*
#include <cmath>
#include <cstdio>
#include <optional>
#include <vector>
#include <iostream>
#include <algorithm>

#include <unordered_map>

using namespace std;

class BusTracker {
    // these two fields provide the input to figure out the next two functions

public:
    BusTracker(std::unordered_map<int, char>& busLocations) : m_busLocations(busLocations) {}

    // given the buses flow in one direction, find how many stops away
    // the next bus is to the provided station
    std::optional<int> nearestBusToStation(std::string stationId) {
        std::optional<int> diff = std::nullopt;
        for(auto it : m_busLocations)
        {
            if(diff == std::nullopt)
            {
                diff = std::strcmp(stationId.c_str(), std::string{it.second}.c_str());
            }
            else if(int diff2 = std::strcmp(stationId.c_str(), std::string{it.second}.c_str()); std::abs(diff2) < std::abs(diff.value()))
            {
                diff = diff2;
            }
        }

        return diff;
    }

    // given a bus id, return the station the bus is currently at
    std::optional<char> getBusLocation(int busId) {
        if(auto it = m_busLocations.find(busId); it == m_busLocations.end())
        {
            return std::nullopt;
        }
        else
        {
            return it->second + static_cast<int>('A');
        }
    };

private:
    std::unordered_map<int, char> m_busLocations;
};

*/





