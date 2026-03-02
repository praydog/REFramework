#pragma once

namespace REFrameworkNET {
namespace Collections {

generic <typename T>
public interface class IEnumerator {
    property T Current { T get(); }
    bool MoveNext();
};

generic <typename T>
public interface class IEnumerable {
    IEnumerator<T>^ GetEnumerator();
};

generic <typename T>
public interface class ICollection {
    property int Count { int get(); }
    void Add(T item);
    void Clear();
    bool Contains(T item);
    bool Remove(T item);
    IEnumerator<T>^ GetEnumerator();
};

generic <typename T>
public interface class IList {
    property int Count { int get(); }
    property T default[int] { T get(int index); void set(int index, T value); }
    void Add(T item);
    void Clear();
    bool Contains(T item);
    bool Remove(T item);
    int IndexOf(T item);
    void Insert(int index, T item);
    void RemoveAt(int index);
    IEnumerator<T>^ GetEnumerator();
};

generic <typename T>
public interface class IReadOnlyCollection {
    property int Count { int get(); }
    IEnumerator<T>^ GetEnumerator();
};

generic <typename T>
public interface class IReadOnlyList {
    property int Count { int get(); }
    property T default[int] { T get(int index); }
    IEnumerator<T>^ GetEnumerator();
};

generic <typename TKey, typename TValue>
public interface class IDictionary {
    property int Count { int get(); }
    property TValue default[TKey] { TValue get(TKey key); void set(TKey key, TValue value); }
    bool ContainsKey(TKey key);
    void Add(TKey key, TValue value);
    bool Remove(TKey key);
    void Clear();
    property ICollection<TKey>^ Keys { ICollection<TKey>^ get(); }
    property ICollection<TValue>^ Values { ICollection<TValue>^ get(); }
};

generic <typename TKey, typename TValue>
public interface class IReadOnlyDictionary {
    property int Count { int get(); }
    property TValue default[TKey] { TValue get(TKey key); }
    bool ContainsKey(TKey key);
    property IEnumerable<TKey>^ Keys { IEnumerable<TKey>^ get(); }
    property IEnumerable<TValue>^ Values { IEnumerable<TValue>^ get(); }
};

} // namespace Collections
} // namespace REFrameworkNET
