// CS 5007, Northeastern University, Seattle
// Summer 2017
//
// Assignment 1

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

struct linkedListNodePtr;
typedef struct linkedListNodePtr LinkedListNodePtr;

struct linkedListNode {
    void *payload;
    LinkedListNodePtr *prev;
    LinkedListNodePtr *next;
};

typedef struct linkedListNode LinkedListNode;


struct linkedListNodePtr {
    LinkedListNode *node;
};


struct linkedListHead {
    int num_nodes;
    LinkedListNodePtr *head;
    LinkedListNodePtr *tail;
};

typedef struct linkedListHead LinkedListHead;

struct linkedList {
    LinkedListHead *listHead;
};

typedef struct linkedList LinkedList;

struct lLIter {
    LinkedListNodePtr *curNode;
};

typedef struct lLIter LLIter;

// ======================================================
// LLIter: A Linked List Iterator
// ======================================================

// Creates a iterator for the given linkedList.
// It is the customer's responsibility to ensure
// that the iterator is destroyed before modifying
// the LinkedList.
// The customer also needs to call DestroyLLIter to free
// it when the customer is done.
//
// INPUT: A pointer to a LinkedList to be iterated.
LLIter* CreateLLIter(LinkedList* list);

// Determines if there are more elements in a given iterator.
//
// INPUT: An existing iterator.
//
// Returns 0 if there are no next elements.
int HasNext(LLIter* iter);

// Determines if there are more elements in a given iterator.
//
// INPUT: An existing iterator.
//
// Returns 0 if there are no previous elements.
int HasPrev(LLIter* iter);

// Returns the payload of the next element in the list
// being iterated by the given iterator.
// The customer is responsible for tracking the
// type of the payloads in a LList.
//
// INPUT: A pointer to an existing iterators.
//
// Returns the payload.
void* Next(LLIter* iter);

// Returns the payload of the prev element in the list
// being iterated by the given iterator.
// The customer is responsible for tracking the
// type of the payloads in a LList.
//
// INPUT: A pointer to an existing iterators.
//
// Returns the payload.
void* Prev(LLIter* iter);

// Destroys and frees the given iterator.
//
// INPUT: A pointer to an existing iterator.
//
// Returns 0 if the iterator was successfully
//   destroyed and freed.
int DestroyLLIter(LLIter* iter);


// Returns the number of elements in the linked list.
int NumElementsLinkedList(LinkedList *list);

// Removes the first element from the list (similar to a pop)
//
// Returns the payload of the head so it can be destroyed
void* RemoveFirstElement(LinkedList *list);

// Gets the payload of the head element.
//
// Returns the payload of the head; the head element is still
// in the list however.
void* GetFirstElement(LinkedList *list);


// ======================================================
// LLIter: A Linked List Iterator
// ======================================================

// Creates a LinkedList.
// The customer is responsible for calling DestroyLinkedList()
// to destroy and free the LinkedList when done.
//
// Returns a pointer to a LinkedList struct.
LinkedList* CreateLinkedList();

// Destroys a LinkedList.
// All structs associated with a LinkedList will be
// released and freed.
// The customer is responsible for ensuring that the
// payloads of the items in the LinkedList are destroyed.
//
// INPUT: A pointer to a LinkedList.
//
// Returns 0 if the destroy was successful.
int DestroyLinkedList(LinkedList* list);

// Adds an element to the tail of a LinkedList, with the payload
// as the given pointer.
// The customer is responsible for allocating the payload.
//
// INPUT: A pointer to the LinkedList that will be added to,
// INPUT: A pointer to a payload for a node in the linkedList.
//
// Returns 0 if the insertion was successful.
int Insert(LinkedList*, void*);

// Removes a given payload from a LinkedList.
// The customer is responsible for free'ing the payload
// after it is removed from the list.
//
// INPUT: A pointer to the LinkedList that will be remove from.
// INPUT: A pointer to the payload to remove from the list.
//
// Returns 0 if the remove was successful.
// An unsuccessful remove includes the case when the given payload
// is not an element in the LinkedList.
int Remove(LinkedList* list, void* data);


void PrintLinkedList(LinkedList* list);

#endif  // LINKEDLIST_H
