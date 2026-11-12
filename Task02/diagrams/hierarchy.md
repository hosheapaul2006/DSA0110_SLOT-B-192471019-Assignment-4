# University UML

```mermaid
classDiagram
class Person {
  -string name_
  -int id_
  -Contact contact_
  +name() const string_ref
  +id() const int
  +~Person() virtual
}
class Contact {
  -string phone_
}
class Programme {
  +string name
  +double cgpa
}
class Student {
  -Programme programme_
}
class Staff {
  -string department_
  -double salary_
}
class ResearchScholar {
  -double fellowship_
}
class TeachingAssistant
Person <|-- Student : public virtual
Person <|-- Staff : public virtual
Student <|-- TeachingAssistant : public
Staff <|-- TeachingAssistant : public
Student <|-- ResearchScholar : public
Person *-- Contact
Student *-- Programme
```

The two virtual edges share exactly one Person within TeachingAssistant. Composition diamonds mean ownership of member objects, not inheritance.

[Standalone UML SVG](hierarchy.svg) is also provided and its rendered layout was reviewed.
