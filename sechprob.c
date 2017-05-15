/*
  This program, supplemental to my Quora "About
  Time, Probably" post in "Off the Vine" blog,
  solves the post's problem:

  - what is the probability that the second hand
    is closer to the minute hand than it is to
    the hour hand when a random look at a 3-hand
    analog clock is taken?

  by hand.

  The computations are carried out in the temporal
  units, namely - seconds (MHR = Minute/Hour hands
  Rendezvous):

	precompute the 11 MHR times;
	while ( elapsed time < 43200 )
	{
		if ( this time frame does not have an MHR )
		{
			add full time delta to the total (m/h)state time;
		}
		else
		{
			add partial time delta to this (m/h)state time;
			add remaining (full - partial) time delta to opposite state time;
			switch the state;
		}
		increment elapsed time;
		move the time frame along;
		switch the state;
	}
 */

#include <stdio.h>

static int		mState = 0;
static int		hState = 1;

static double		MHR[ 11 ] = { 0 };
static size_t		MHRSZ = sizeof( MHR ) / sizeof( MHR[ 0 ] );

static void		initMHR();
static int		hasMHR( double, double );

extern int
main( int argc, char *argv[] )
{
	int		state;
	int		mhrndx = -1;
	int		minuteN = 0;
	int		hourN = 0;
	double		elapsedtm = 0.0;

	/*
	  Since the second hand spends all of its time
	  in either semicircle delineated by the rotating
	  MHAB, the two variables below keep track (in
	  absolute units) of the boundaries of the time
	  frame associated with the second hand's dwelling
	  in this particular semicircle:

	  'scEnterTm' keeps track of when the second hand
	  enters this semicircle

	  'scLeaveTm' keeps track of when the second hand
	  leaves this semicircle
	 */
	double		scEnterTm = 0.0;
	double		scLeaveTm = 0.0;

	double		minutetm = 0.0;
	double		hourtm = 0.0;
	double		partialdelta = 0.0;
	double		remainingdelta = 0.0;
	double		fulldelta = 43200.0 / 1427.0;

	initMHR();

	state = mState;
	scLeaveTm = fulldelta;

	while ( ( int )elapsedtm < 43200 )
	{
		mhrndx = hasMHR( scEnterTm, scLeaveTm );
		if ( mhrndx < 0 )
		{
			if ( state == mState )
			{
				minutetm += fulldelta;
				minuteN++;
			}
			else
			{
				hourtm += fulldelta;
				hourN++;
			}
		}
		else
		{
			partialdelta = MHR[ mhrndx ] - scEnterTm;
			remainingdelta = fulldelta - partialdelta;

			if ( state == mState )
			{
				minutetm += partialdelta;
				minuteN++;
				hourtm += remainingdelta;
				hourN++;
			}
			else
			{
				hourtm += partialdelta;
				hourN++;
				minutetm += remainingdelta;
				minuteN++;
			}

			state = state == mState ? hState : mState;
		}
		elapsedtm += fulldelta;
		scEnterTm = scLeaveTm;
		scLeaveTm += fulldelta;
		state = state == mState ? hState : mState;
	}
	minutetm -= ( elapsedtm - 43200.0 ); /* small floating point arithmetic discrepancy */
	state = state == mState ? hState : mState;

	printf( "\nfulldelta = %12.9f\n"
		"(S_m) m-state duration = %f secs, N = %d\n"
		"(S_h) h-state duration = %f secs, N = %d\n"
		"S_m + S_h = %f secs\n"
		"S_m - S_h = %11.9f secs\n"
		"last state was %s\n"
		"P(minute) = %11.9f\n"
		"P(hour) = %11.9f\n",
			fulldelta,
			minutetm, minuteN,
			hourtm, hourN,
			minutetm + hourtm,
			minutetm - hourtm,
			state == mState ? "m-state" : "h-state",
			minutetm / 43200.0,
			hourtm / 43200.0 );

	return 0;
}

/*
  The MHR coordinates are stored in absolute
  temporal units - seconds measured from the
  12-o'clock mark.

  For display purposes these units are converted
  into a more readable hh:mm:ss.nnnnnnnnn format
 */
static void
initMHR()
{
	int		n;
	int		hrs;
	int		mins;
	double		secs;

	for ( n = 1; n <= MHRSZ; n++ )
	{
		hrs = n;
		mins = ( n * 60 ) / 11;
		secs = ( ( n * 60.0 ) / 11.0 - mins ) * 60;
		MHR[ n - 1 ] = 3600 * hrs + 60 * mins + secs;
	}

	printf( "MHRs:\n" );
	for ( n = 0; n < MHRSZ; n++ )
	{
		hrs = MHR[ n ] / 3600;
		mins = ( MHR[ n ] - hrs * 3600 ) / 60;
		secs = MHR[ n ] - hrs * 3600 - mins * 60;
		printf( "%02d:%02d:%012.9f\n", hrs, mins, secs );
	}
}

static int
hasMHR( double scEnterTm, double scLeaveTm )
{
	int		n;

	for ( n = 0; n < ( MHRSZ - 1 ); n++ )
	{
		if ( MHR[ n ] > scEnterTm && MHR[ n ] < scLeaveTm )
		{
			return n;
		}
	}

	return -1;
}
