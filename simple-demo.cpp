#include <cstdlib>
#include <cstdio>

#include "interval-tree.hpp"


#define BOUNDS 2
#if BOUNDS == 1
	#define LEFT -5.0f
	#define RIGHT 5.0f
#elif BOUNDS == 2
	#define LEFT -0.2f/9.0f
	#define RIGHT 1.2f/9.0f
#endif

inline static float uniform(void) {
	return (float)rand() / (float)RAND_MAX;
}
inline static float uniform(float l,float r) {
	return l + (r-l)*uniform();
}

static void print_point(float x) {
	float part = (x-LEFT)/(RIGHT-LEFT);
	int i_x = (int)(part*79.0f+0.5f);
	for (int i=0;i<i_x;++i) printf(" ");
	printf("|\n");
}
static void print_interval(std::interval_tree<1,float>::interval const& i) {
	float part_l = (i.left -LEFT)/(RIGHT-LEFT);
	float part_r = (i.right-LEFT)/(RIGHT-LEFT);
	int i_l = (int)(part_l*79.0f+0.5f);
	int i_r = (int)(part_r*79.0f+0.5f);

	for (int i=0;i<i_l;++i) printf(" ");
	if (i_l<i_r) {
		printf("<");
		for (int i=i_l+1;i<i_r;++i) printf("-");
		printf(">\n");
	} else {
		printf("x\n");
	}
}
static void print_intervals(std::vector<std::interval_tree<1,float>::interval> const& intervals) {
	for (std::interval_tree<1,float>::interval i : intervals) {
		print_interval(i);
	}
}

int main(int /*argc*/, char* /*argv*/[]) {
	{
		#define TREE 2
		#if TREE == 1
			std::vector<std::interval_tree<1,float>::interval> intervals;
			srand(1); for (int i=0;i<20;++i) {
				float a = uniform(LEFT,RIGHT);
				float b = uniform(a,RIGHT);
				intervals.push_back({a,b});
			}
			std::interval_tree<1,float> tree(intervals);
		#elif TREE == 2
			std::vector<std::interval_tree<1,float>::interval> intervals;
			for (size_t i=0;i<10;++i) {
				intervals.push_back({
					static_cast<float>(i  ) / 90.0f,
					static_cast<float>(i+1) / 90.0f
				});
			}
			std::interval_tree<1,float> tree(intervals);
		#endif

		#define QUERY 3
		#if QUERY == 1
			float test_x = uniform(LEFT,RIGHT);

			printf("Intervals:\n"); print_intervals(intervals);

			printf("Query:\n"); print_point(test_x);

			printf("Intersects:\n");
			std::vector<std::interval_tree<1,float>::interval> result;
			tree.intersect(test_x,&result);
			print_intervals(result);
		#elif QUERY == 2
			std::interval_tree<1,float>::interval query;
			query.left = uniform(LEFT,RIGHT);
			query.right = uniform(query.left,RIGHT);

			printf("Intervals:\n"); print_intervals(intervals);

			printf("Query:\n"); print_interval(query);

			printf("Intersects:\n");
			std::vector<std::interval_tree<1,float>::interval> result;
			tree.intersect(query,&result);
			print_intervals(result);
		#elif QUERY == 3
			std::interval_tree<1,float>::interval query;
			//query.left=-0.0166664999f; query.right=0.0277774986f;
			query.left= 0.0277759954f; query.right=0.0722239986f;

			printf("Intervals:\n"); print_intervals(intervals);

			printf("Query:\n"); print_interval(query);

			printf("Intersects:\n");
			std::vector<std::interval_tree<1,float>::interval> result;
			tree.intersect(query,&result);
			print_intervals(result);
		#endif
	}
	printf("ENTER to exit.\n"); getchar();
	return 0;
}
