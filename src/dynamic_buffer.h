#ifndef DYNAMIC_BUFFER_H
#define DYNAMIC_BUFFER_H

#include <stdint.h>

struct SimulatorData;

class DynamicBufferArray {
public:
    void addObject(const SimulatorData *obj);
    void addObject(SimulatorData obj);
    void removeObject(uint32_t index);
    void editObject(uint32_t index, const SimulatorData *obj);
    void editObject(uint32_t index, SimulatorData obj);

    uint32_t buffObject() const;
    uint32_t size() const;

    DynamicBufferArray();

private:
    void realloc(uint32_t length);

    uint32_t m_length;
    uint32_t m_capacity;
    uint32_t m_buffObject;
};

#endif // !DYNAMIC_BUFFER_H
