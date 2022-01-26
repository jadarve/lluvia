---
title: "A first example"
date: 2021-06-12T00:00:00-05:00
weight: 3
draft: true
---


$$
    \tag*{(1)} P(E) = {n \choose k} p^k (1-p)^{n-k}
$$


```mermaid
graph LR
  Start --> Need{"Do I need diagrams"}
  Need -- No --> Off["Set params.mermaid.enable = false"]
  Need -- Yes --> HaveFun["Great!  Enjoy!"]
```

{{< tabpane >}}
  {{< tab header="Python" lang="python">}}
import lluvia as ll

session = ll.createSession()
{{< /tab >}}
{{< tab header="C++" lang="c++">}}
#include <lluvia/core.h>

int main() {
    auto session = ll::Session::create();

    return 0;
}
{{< /tab >}}
{{< /tabpane >}}

```cpp
#include <lluvia/core.h>

int main() {
    auto session = ll::Session::create();

    return 0;
}
```

```python
import lluvia as ll

session = ll.createSession()
```

moni
