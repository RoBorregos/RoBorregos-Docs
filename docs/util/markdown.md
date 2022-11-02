# Getting Started with Markdown

Markdown is a simple markup language that allows you to write using a simple sintax. It's used in many places because of how easy it's to use, understand and read.

## Headings

Headings are created using the `#` symbol. The more `#` you use, the smaller the heading will be.

Example:
```markdown
# Heading 1
## Heading 2
### Heading 3
#### Heading 4
```

## Text

Text is written as it is. You can use **bold** and *italic* text. You can also use ~~strikethrough~~ text. 

Example:
```markdown
This is a normal text. You can use **bold** and *italic* text. You can also use ~~strikethrough~~ text. 
```

## Lists

You can create lists using the `-` symbol. You can also create numbered lists using the `1.` symbol.

Example:
```markdown
- Item 1
- Item 2
    - Item 2.1
    - Item 2.2
1. Item 1
2. Item 2
    1. Item 2.1
    2. Item 2.2
```
Example output:

- Item 1
- Item 2
    - Item 2.1
    - Item 2.2
1. Item 1
2. Item 2
    1. Item 2.1
    2. Item 2.2

## Links

You can create links using the `[text](link)` sintax.

Example:
```markdown
[RoBorregos](
    https://www.roborregos.com
)
```
Example output:
[RoBorregos](https://www.roborregos.com)

## Images

Similar to links, you can add images using the `![alt text](image link)` sintax.

Example:
```markdown
![RoBorregos Logo](https://github.com/RoBorregos.png)
```
Example output:
![RoBorregos Logo](https://github.com/RoBorregos.png)

## Code

You can add code using the `` ` `` symbol. You can also add code blocks using the ``` symbol.

Example:
```markdown
`print("Hello World")`
```
Example output:
`print("Hello World")`

Example:
```markdown
    ```python
    print("Hello World")
    ```
```
Example output:
```python
print("Hello World")
```

## Tables

You can create tables using the `|` symbol.

Example:
```{markdown}
| Name | Email | Role |
| ---- | ----- | ---- |
| Ivan | [i.wells.ar@gmail.com](mailto:i.wells.ar@gmail.com) | Software Developer, Repo Mantainer and Automatization |

```

Example output:

| Name | Email | Role |
| ---- | ----- | ---- |
| Ivan | [i.wells.ar@gmail.com](mailto:i.wells.ar@gmail.com) | Software Developer, Repo Mantainer and Automatization |

## Quotes

You can create quotes using the `>` symbol.

Example:
```{markdown}
> This is a quote
```
Example output:
> This is a quote

## Horizontal Rule

You can create a horizontal rule using the `---` symbol.

Example:
```{markdown}
---
```
Example output:

---

## ToDo List

You can create a task list using the `- [ ]` symbol.

Example:
```{markdown}
- [ ] ToDo 
- [x] Done ToDo
```
Example output:

- [ ] ToDo
- [x] Done ToDo


