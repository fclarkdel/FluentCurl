# Specification

## 1. Definitions

1. The key words **MUST**, **MUST NOT**, **REQUIRED**, **SHALL**, **SHALL
   NOT**, **SHOULD**, **SHOULD NOT**, **RECOMMENDED**,  **MAY**, and
   **OPTIONAL** in this document are to be interpreted as described in
   _RFC 2119_.
1. A **session** refers to an object that performs network transfers concurrently, and shares state amongst handles.
1. A **handle** refers to a configurable network transfer object that can be used to invoke a network transfer when passed to a 
   **session**. Specifically this will be the analog to the "easy handle" from _libcurl_.

## 2. Functional requirements

1. User **MUST** be able to specify a **session**.
1. User **MUST** be able to specify a **handle** with all of the options available in _libcurl_ (except those
   necessarily reserved for _fluent_curl_ functionality) through a fluent interface.

## 3. Non-functional requirements

1. A **session** **MUST** engage in resource sharing where effective.
1. A **session** **MUST** be thread safe.
1. The state of a **handle** **MUST** be idempotent on repeated invokation.