% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/getPerson.R
\name{getPerson}
\alias{getPerson}
\title{Retrieve person object after running createShinyGUI}
\usage{
getPerson()
}
\description{
This function returns a suitable person object identical to the result returned by \code{\link{mirtCAT}},
and is only required when the GUI is launched by the \code{\link{createShinyGUI}} method.
}
\examples{
\dontrun{

mirtCAT_preamble(df = df)
runApp(createShinyGUI(), port = 8000)

person <- getPerson()
summary(person)
} 
}
\author{
Phil Chalmers \email{rphilip.chalmers@gmail.com}
}
\seealso{
\code{\link{mirtCAT}}, \code{\link{mirtCAT_preamble}}, \code{\link{createShinyGUI}}
}

