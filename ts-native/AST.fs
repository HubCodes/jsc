module rec ts_native.AST

open ts_native.Type

type ID =
  | ID of
      content: string *
      loc: Loc

type PrivateID =
  | PrivateID of
      content: string *
      loc: Loc

[<RequireQualifiedAccess>]
type Program = Stmt list

[<RequireQualifiedAccess>]
type Stmt =
  | Empty
  | Block of
      body: List<Stmt> *
      loc: Loc
  | Var of
      kind: VarKind *
      decl: List<VarDecl>

[<RequireQualifiedAccess>]
type VarKind =
  | Let
  | Const

type VarDecl =
  | VarDecl of
      name: BindName *
      isDefiniteAssignment: bool *
      typeAnnotation: Option<TypeNode> *
      init: Option<Expr> *
      loc: Loc

type TypeNode =
  | Keyword of unit

type Expr = unit

[<RequireQualifiedAccess>]
type BindName =
  | ID of ID
  | Pattern of BindPattern

[<RequireQualifiedAccess>]
type BindPattern =
  | Obj of List<BindElem>
  | Array of List<ArrayBindElem>

type BindElem =
  | BindElem of
      propName: PropName *
      name: BindName *
      init: Option<Expr> *
      isSpread: bool *
      loc: Loc

type ArrayBindElem =
  | BindElem of BindElem
  | Omitted of OmittedExpr

type OmittedExpr =
  | OmittedExpr

[<RequireQualifiedAccess>]
type PropName =
  | ID of ID
  | String of StringLiteral
  | Numeric of NumericLiteral
  | Computed of ComputedPropName
  | Private of PrivateID

type ComputedPropName =
  | ComputedPropName of
      expr: Expr *
      loc: Loc

[<RequireQualifiedAccess>]
type LiteralExpr =
  | String of StringLiteral
  | RegExp of RegExpLiteral
  | NoSubstitutionTemplate of NoSubstitutionTemplate
  | Numeric of NumericLiteral
  | BigInt of BigIntLiteral

type StringLiteral =
  | StringLiteral of
      text: string *
      loc: Loc

type RegExpLiteral =
  | RegExpLiteral of
      text: string *
      loc: Loc

type NoSubstitutionTemplate =
  | NoSubstitutionTemplate of
      text: string *
      loc: Loc

type NumericLiteral =
  | NumericLiteral of
      text: string *
      value: double *
      isDouble: bool *
      loc: Loc

type BigIntLiteral =
  | BigIntLiteral of
      text: string *
      value: bigint *
      loc: Loc
