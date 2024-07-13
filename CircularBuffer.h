#pragma once
#include <vector>
#include <iostream>

template <typename T>
class CircularBuffer {
public:
	CircularBuffer() {}

	CircularBuffer(size_t size)
		: m_maxSize{ size }
		, m_vector(size) {}

	CircularBuffer(size_t size, T value)
		: m_maxSize{ size }
		, m_vector(size, value) {}


	template <typename U>
	void push_back(U&& value);
	void pop();
	void back();
	void forth();
	void resize(size_t newMaxSize);
	void clear();
	size_t circleDistance() const;

	T& head() {
		return m_vector[m_head];
	}

	size_t size() const {
		return m_size;
	}

	size_t maxSize() const {
		return m_maxSize;
	}

private:
	std::vector<T> m_vector;

	size_t m_maxSize = 0;
	size_t m_size    = 0;
	size_t m_head    = 0;
	size_t m_tail    = 0;

	size_t circleIncrement(size_t value) const;
	size_t circleDecrement(size_t value) const;
};



template <typename T>
template <typename U>
void CircularBuffer<T>::push_back(U&& value) {
	if (m_maxSize == 0) {
		std::cerr << "CircularBuffer::push_back: buffer capacity is zero\n";
		return;
	}

	if (m_size != 0) {
		m_head = circleIncrement(m_head);
	}

	if (m_size == m_maxSize && m_head == m_tail) {
		m_tail = circleIncrement(m_tail);
	}
	else {
		m_size = circleDistance() + 1;
	}

	m_vector[m_head] = std::forward<U>(value);
}

template <typename T>
void CircularBuffer<T>::pop() {
	if (m_head == m_tail) {
		return;
	}
	m_vector[m_head] = {};
	m_head = circleDecrement(m_head);
	m_size = circleDistance() + 1;
}

template <typename T>
void CircularBuffer<T>::back() {
	if (m_head == m_tail) {
		return;
	}
	m_head = circleDecrement(m_head);
}

template <typename T>
void CircularBuffer<T>::forth() {
	if (m_head == circleDecrement(m_tail) || circleDistance() + 1 == m_size) {
		return;
	}
	m_head = circleIncrement(m_head);
}

template <typename T>
void CircularBuffer<T>::resize(size_t newMaxSize) {
	if (newMaxSize == 0) {
		clear();
		return;
	}

	std::vector<T> newVector(newMaxSize);
	size_t newSize = 0;

	if (m_size != 0) {
		for (auto& value : newVector) {
			value = std::move(m_vector[m_head]);
			newSize++;

			if (m_head == m_tail) {
				break;
			}
			m_head = circleDecrement(m_head);
		}
	}

	m_maxSize = newMaxSize;
	m_size = newSize;
	m_head = m_size == 0 ? 0 : m_size - 1;
	m_tail = 0;
	m_vector = std::move(newVector);
}

template <typename T>
void CircularBuffer<T>::clear() {
	m_maxSize = 0;
	m_size = 0;
	m_head = 0;
	m_tail = 0;
	m_vector.clear();
}


template <typename T>
size_t CircularBuffer<T>::circleDistance() const {
	if (m_head >= m_tail) {
		return m_head - m_tail;
	}
	else {
		return m_maxSize - m_tail + m_head;
	}
}

template <typename T>
size_t CircularBuffer<T>::circleIncrement(size_t value) const {
	if (value == m_maxSize - 1) {
		return 0;
	}
	else {
		return value + 1;
	}
}

template <typename T>
size_t CircularBuffer<T>::circleDecrement(size_t value) const {
	if (value == 0) {
		return m_maxSize - 1;
	}
	else {
		return value - 1;
	}
}