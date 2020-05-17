module Typische.SourceCode

type Loc =
  { StartLine: int
    StartCol: int
    EndLine: int
    EndCol: int }
with
  static member ZeroPos () =
    { StartLine = 0
      StartCol = 0
      EndLine = 0
      EndCol = 0 }
