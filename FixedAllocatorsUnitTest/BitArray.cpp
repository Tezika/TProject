#include "pch.h"
#include "BitArray.h"
#include "FixedSizeAllocator.h"
#include "Assert.h"
#include <cstring>
#include <math.h>
#include <intrin.h>

#pragma intrinsic(_BitScanForward) 

BitArray * BitArray::Create(size_t i_numBits, FixedSizeAllocator * i_pAllocator, bool i_startClear)
{
	return new BitArray(i_numBits, i_startClear);
}

BitArray::BitArray(size_t i_numBits, bool i_clearAll)
	:m_numOfBits(i_numBits)
{
	float i_bitsPerUnit = 8;
	m_sizeOfUnits = ceil((float)(i_numBits / i_bitsPerUnit));
	m_pBits = new uint8_t[m_sizeOfUnits];
	assert(m_pBits);
	memset(m_pBits, i_clearAll ? 0 : UINT8_MAX, m_sizeOfUnits);
}

BitArray::BitArray(const BitArray & i_other)
	:m_numOfBits(i_other.m_numOfBits)
{
	m_sizeOfUnits = i_other.GetUnitSize();
	if (m_pBits != nullptr)
	{
		delete m_pBits;
		m_pBits = nullptr;
	}
	m_pBits = new uint8_t[m_sizeOfUnits];
	assert(m_pBits);
	memcpy_s(m_pBits, GetUnitSize(), i_other.m_pBits, i_other.GetUnitSize());
}

BitArray & BitArray::operator=(const BitArray & i_other)
{
	m_sizeOfUnits = i_other.GetUnitSize();
	if (m_pBits != nullptr)
	{
		delete m_pBits;
		m_pBits = nullptr;
	}
	m_pBits = new uint8_t[m_sizeOfUnits];
	assert(m_pBits);
	memcpy_s(m_pBits, GetUnitSize(), i_other.m_pBits, i_other.GetUnitSize());
	return *this;
}

BitArray::~BitArray()
{
	assert(!this->AreAllClear());
	if (m_pBits != nullptr)
	{
		delete m_pBits;
		m_pBits = nullptr;
	}
}

void BitArray::SetAll()
{
	memset(m_pBits, UINT8_MAX, m_sizeOfUnits);
}

void BitArray::ClearAll()
{
	memset(m_pBits, 0, m_sizeOfUnits);
}

bool BitArray::AreAllClear() const
{
	size_t bitNumber = -1;
	return !this->GetFirstSetBit(bitNumber);
}

bool BitArray::AreAllSet() const
{
	size_t bitNumber = -1;
	return !this->GetFirstClearBit(bitNumber);
}

void BitArray::SetBit(size_t i_bitNumber)
{
	assert(i_bitNumber < m_numOfBits);
	size_t bit = i_bitNumber % 8;
	size_t unitIdx = i_bitNumber / 8;
	uint8_t unit = m_pBits[unitIdx];
	unit |= (1U << bit);
	m_pBits[unitIdx] = unit;
}

void BitArray::ClearBit(size_t i_bitNumber)
{
	assert(i_bitNumber < m_numOfBits);
	size_t bit = i_bitNumber % 8;
	size_t unitIdx = i_bitNumber / 8;
	uint8_t unit = m_pBits[unitIdx];
	unit &= ~(1U << bit);
	m_pBits[unitIdx] = unit;
}

bool BitArray::GetFirstClearBit(size_t & o_bitNumber) const
{
	size_t unitIndex = m_sizeOfUnits;
	size_t bitIndex = 0;
	bool foundBit = false;
	for (size_t i = 0; i < m_sizeOfUnits; i++)
	{
		if (m_pBits[i] == UINT8_MAX)
		{
			continue;
		}
		for (size_t bit = 0; bit < 8; bit++)
		{
			if (this->IsBitClear(i * 8 + bit))
			{
				unitIndex = i;
				bitIndex = bit;
				foundBit = true;
				break;
			}
		}
		if (foundBit)
		{
			break;
		}
	}
	o_bitNumber = unitIndex * 8 + bitIndex;
	return o_bitNumber != m_numOfBits;
}

bool BitArray::GetFirstSetBit(size_t & o_bitNumber) const
{
	size_t unitIndex = 0;
	// quick skip bytes where no bits are set   
	while ((m_pBits[unitIndex] == 0) && (unitIndex < m_sizeOfUnits))
		unitIndex++;
	if (unitIndex == m_sizeOfUnits)
	{
		return false;
	}
	// use the compiler intrinsics function to find the first set bit.
	unsigned long unit = m_pBits[unitIndex];
	unsigned long bitIndex;
	_BitScanForward(&bitIndex, unit);
	o_bitNumber = unitIndex * 8 + bitIndex;
	return o_bitNumber != m_numOfBits;
}

bool BitArray::operator[](size_t i_index) const
{
	return this->IsBitSet(i_index);
}
