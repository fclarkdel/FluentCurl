# Specification

## 1. Definitions

> 1. The key words **MUST**, **MUST NOT**, **REQUIRED**, **SHALL**, **SHALL
     NOT**, **SHOULD**, **SHOULD NOT**, **RECOMMENDED**,  **MAY**, and
     **OPTIONAL** in this document are to be interpreted as described in
     _RFC 2119_.
> 1. A **transfer** refers to any communication over a network initiated by libcurl.
> 1. A **result** refers to any data/ metadata associated with a specific **transfer**,
     such as the result of an HTTP POST request or how long it took the **transfer** to connect to the host.

## 2. Functional requirements

> 1. User **MUST** be able to specify a factory through which **transfers** are created.
> 1. User **MUST** be able to specify a **transfer** with options directly mapped to the options offered by libcurl -
     except for options which must be reserved for FluentCurl functionality.
> 1. User **MUST** asynchronously receive a **result** after a performed **transfer** has completed or failed.
> 1. The state of **transfers** **MUST** be idempotent on repeated performance -
     meaning the state the user places a **transfer** in must be maintained until the user decides to change it.
> 1. User **MUST** be able to perform a **transfer** multiple times before any previous has completed.

## 3. Non-functional requirements

> 1. All libcurl resources **MUST** be idiomatically handled by scope bound resource management -
     including the global state introduced by libcurl.
> 1. **Transfers** created from the same factory **MUST** engage in resource sharing where effective.