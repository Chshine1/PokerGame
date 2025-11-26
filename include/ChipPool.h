#ifndef CHIP_POOL_H
#define CHIP_POOL_H

class ChipPool {
public:
    explicit ChipPool(int initialChips = 1000);

    bool takeChips(int amount);

    void addChips(int amount);

    int getChips() const;

private:
    int chips_;
};

#endif
