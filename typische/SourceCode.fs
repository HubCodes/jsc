module Typische.SourceCode

type Loc =
  { startLine: int
    startCol: int
    endLine: int
    endCol: int }
with
  static member ZeroPos () =
    { startLine = 0
      startCol = 0
      endLine = 0
      endCol = 0 }
