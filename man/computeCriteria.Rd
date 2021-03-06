% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/computeCriteria.R
\name{computeCriteria}
\alias{computeCriteria}
\title{Compute the values given the criteria and internal objects}
\usage{
computeCriteria(x, criteria, person = NULL, test = NULL, design = NULL)
}
\arguments{
\item{x}{an object of class 'mirtCAT_design' returned from the \code{\link{mirtCAT}} function
when passing \code{design_elements = TRUE}}

\item{criteria}{item selection criteria (see \code{\link{mirtCAT}}'s \code{criteria} input)}

\item{person}{(required when \code{x} is missing) internal person object. To be 
used when \code{customNextItem} function has been defined}

\item{test}{(required when \code{x} is missing) internal test object. To be 
used when \code{customNextItem} function has been defined}

\item{design}{(required when \code{x} is missing) internal design object. To be 
used when \code{customNextItem} function has been defined}
}
\value{
a vector of criteria values for each respective item
}
\description{
A function that returns a vector of evaluated criteria for each respective item in the 
test bank. Note that criteria values are returned such that the maximum value always 
represents the most optimal item (e.g., maximum information). In cases where the minimum value is 
typically selected (e.g., minimum variance) all values are multiplied by -1 to turn it into a maximization
problem.
}
\examples{
\dontrun{
# test defined in mirtCAT help file, first example
CATdesign <- mirtCAT(df, mod, design_elements = TRUE)

computeCriteria(CATdesign, criteria = 'MI')
computeCriteria(CATdesign, criteria = 'MEI')

}
}
\author{
Phil Chalmers \email{rphilip.chalmers@gmail.com}
}
\seealso{
\code{\link{mirtCAT}}, \code{\link{updateDesign}}, \code{\link{extract.mirtCAT}},
  \code{\link{findNextItem}}
}

