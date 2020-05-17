module rec Typische.Syntax

open Typische.SourceCode

type ID = ID of string

type Program =
  { fileName: string
    listing: List<Stmt> }

type Stmt =
  | Block of
    block: Block *
    loc: Loc
  | VarDec of
    varDec: VarDec *
    loc: Loc
  | FuncDec of
    funcDec: FuncDec *
    loc: Loc
  | TypeDec of
    typeDec: TypeDec *
    loc: Loc
  | InterfaceDec of
    interfaceDec: InterfaceDec *
    loc: Loc
  | ClassDec of
    classDec: ClassDec *
    loc: Loc
  | Expr of
    expr: Expr *
    loc: Loc

type Block = List<Stmt>

type VarDec =
  | VarDec of
    kind: VarKind *
    dec: VarDecBody *
    loc: Loc

type FuncDec =
  | FuncDec of
    name: Option<ID> *
    isAsync: bool *
    isGenerator: bool *
    isArrow: bool *
    code: Block *
    args: List<VarDecBody> *
    funcType: FuncType *
    loc: Loc

type TypeDec =
  | TypeDec of
    name: ID *
    typeParams: List<TypeParamDecl> *
    dec: TypeObject *
    loc: Loc

type InterfaceDec =
  | InterfaceDec of
    name: ID *
    typeParams: List<TypeParamDecl> *
    heritage: List<Heritage> *
    dec: InterfaceBody *
    loc: Loc

type ClassDec =
  | ClassDec of
    name: Option<ID> *
    typeParams: List<TypeParamDecl> *
    heritage: List<Heritage> *
    members: List<ClassMember> *
    loc: Loc

type Expr =
  | ArrayLiteral of
    array: ArrayLiteral *
    loc: Loc
  | ObjectLiteral of
    object: ObjectLiteral *
    loc: Loc
  | PropertyAccess of
    propertyAccess: PropertyAccess *
    loc: Loc
  | ElementAccess of
    elementAccess: ElementAccess *
    loc: Loc
  | Call of
    call: Call *
    loc: Loc
  | New of
    newExpr: New *
    loc: Loc
  | TypeAssertion of
    typeAssertion: TypeAssertion *
    loc: Loc
  | Func of
    funcDec: FuncDec *
    loc: Loc
  | Delete of
    delete: Delete *
    loc: Loc
  | TypeOf of
    typeOf: TypeOf *
    loc: Loc
  | Await of
    await: Await *
    loc: Loc
  | PrefixUnary of
    prefixUnary: PrefixUnary *
    loc: Loc
  | PostfixUnary of
    postfixUnary: PostfixUnary *
    loc: Loc
  | Binary of
    binary: Binary *
    loc: Loc
  | Conditional of
    conditional: Conditional *
    loc: Loc
  | Yield of
    yieldExpr: Yield *
    loc: Loc
  | Class of
    classExpr: ClassDec *
    loc: Loc

type VarDecBody =
  | VarDecBody of
    name: VarName *
    typeAnnotation: Option<Type> *
    init: Option<Expr> *
    loc: Loc

type VarKind =
  | Let
  | Const

[<RequireQualifiedAccess>]
type Type =
  | Keyword of KeywordType
  | This
  | Func of FuncType
  | Array of ArrayType
  | Tuple of TupleType
  | Optional of OptionalType
  | Union of UnionType
  | Intersection of IntersectionType
  | Literal of LiteralType
  | Named of NamedType
  | Unknown

type FuncType =
  | FuncType of
    args: List<Type> *
    returns: Type
